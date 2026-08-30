#!/usr/bin/env bash

client_log_path="$HOME/.local/share/mana/manaverse.log"
screenshot_dir="$HOME/Desktop/ManaVerse"

check_is_client_running() {
    if ! kill -0 ${PID}; then
        echo "Error: process look like crashed"
        cat logs/run.log
        echo "Run with gdb"
        cp ./src/manaplus ./logs/
        cp -r core* ./logs/
        sleep 10
        COREFILE=$(find . -maxdepth 1 -name "core*" | head -n 1)
        if [[ -f "$COREFILE" ]]; then
            gdb -c "$COREFILE" ./src/manaplus -ex "thread apply all bt" -ex "set pagination 0" -batch
        fi
        exit 1
    fi
}

check_for_asserts() {
    grep -A 20 "Assert:" "$client_log_path"
    if [ "$?" == 0 ]; then
        echo "Assert found in log"
        exit 1
    fi
}

wait_for_server_list() {
    for ((loop=0; loop < 250; ++loop)); do
        [[ $loop -gt 0 ]] && sleep 3
        check_is_client_running
        # check here
        grep "Skipping servers list update" "$client_log_path" && return
        grep "Servers list updated" "$client_log_path" && return
        grep "Error: servers list updating error" "$client_log_path"
        if [ "$?" == 0 ]; then
            echo "Servers list downloading error"
            exit 1
        fi
        printf "Waiting for MV to update server list (loop %d)..\n" $loop
    done
    printf "Error: server list update not completed in alloted time\n"
    exit 1
}

run_client() {
    ./src/manaplus --hide-cursor --enable-ipc --renderer=0 >logs/run.log 2>&1 &
    export PID=$!
    local w=1
    printf "ManaVerse PID: %d. Waiting %d seconds...\n" "$PID" $w
    sleep $w
    wait_for_server_list
    check_for_asserts
}

kill_client() {
    kill -s SIGTERM ${PID}
    export RET=$?

    sleep 10

    if [ "${RET}" != 0 ]; then
        printf "Error: process not responding to SIGTERM, using SIGKILL\n"
        kill -s SIGKILL ${PID}
        sleep 1
        cat logs/run.log
        exit 1
    fi
}

print_run_log() {
    if [[ ! -s logs/run.log ]]; then
        printf "Error: no output in logs/run.log\n"
        exit 1
    fi

    cat logs/run.log

    if grep "[.]cpp" logs/run.log; then
        echo "Error: possible leak detected, see above"
        exit 1
    fi
}

send_command() {
    echo -n "$1" | nc 127.0.0.1 44007
    sleep 5
    check_is_client_running
    check_for_asserts
}

wait_for_file() {
    for ((loop = 0; loop < 7; loop++)); do
        if [[ -f "$1" ]]; then
            return 0
        fi
        sleep 1
    done
    printf "Error: file '%s' did not show up in alloted time\n" "$1"
    exit 1
}

screenshot_diff() {
    local path1="$screenshot_dir/$1"
    local path2="$screenshot_dir/$2"
    wait_for_file "$path1"
    wait_for_file "$path2"
    diff -q "$path1" "$path2" >> /dev/null
    return $?
}

imagesdiff() {
    if screenshot_diff "$1" "$2"; then
        printf "Error: images '%s' and '%s' are the same.\n" "$1" "$2"
        exit 1
    fi
}

imagessame() {
    if ! screenshot_diff "$1" "$2"; then
        printf "Error: images '%s' and '%s' are different.\n" "$1" "$2"
        exit 1
    fi
}

if [[ -z "${ABOUTYOFFSET}" ]]; then
    export ABOUTYOFFSET="395"
fi

run_client
check_is_client_running

send_command "/screenshot run.png"

# send down key
send_command "/guikey -960 keyGUIDown"
send_command "/screenshot downkey.png"
imagesdiff run.png downkey.png

# send up key
send_command "/guikey -961 keyGUIUp"
send_command "/screenshot run2.png"
imagessame run.png run2.png

# open help
send_command "/help"
send_command "/screenshot help.png"
imagesdiff run2.png help.png
send_command "/help"
send_command "/screenshot run3.png"
imagessame run2.png run3.png

# open settings
send_command "/setup"
send_command "/screenshot setup.png"
imagesdiff run3.png setup.png
send_command "/setup"
send_command "/screenshot run4.png"
imagessame run3.png run4.png

# set focus to servers window
send_command "/sendmousekey 400 300 1"
send_command "/screenshot center_click.png"
imagesdiff run4.png center_click.png

send_command "/guikey -989 keyGUIHome"
send_command "/screenshot run5.png"
imagessame run4.png run5.png

# open add server dialog
send_command "/guikey -990 keyGUIInsert"
send_command "/screenshot add_server_empty.png"
imagesdiff run5.png add_server_empty.png

send_command "/sendchars local"
send_command "/screenshot add_server_name.png"
imagesdiff add_server_empty.png add_server_name.png

send_command "/guikey 9 keyGUITab"
send_command "/screenshot add_server_adress1.png"
imagesdiff add_server_name.png add_server_adress1.png

send_command "/sendchars 127.0.0.1"
send_command "/screenshot add_server_adress2.png"
imagesdiff add_server_adress1.png add_server_adress2.png

send_command "/guikey 9 keyGUITab"
send_command "/screenshot add_server_port.png"
imagesdiff add_server_adress2.png add_server_port.png

send_command "/guikey 9 keyGUITab"
send_command "/screenshot add_server_type.png"
imagesdiff add_server_port.png add_server_type.png

send_command "/guikey 9 keyGUITab"
send_command "/screenshot add_server_description1.png"
imagesdiff add_server_type.png add_server_description1.png

send_command "/sendchars local server"
send_command "/screenshot add_server_description2.png"
imagesdiff add_server_description1.png add_server_description2.png

send_command "/guikey 9 keyGUITab"
send_command "/screenshot add_server_url1.png"
imagesdiff add_server_description2.png add_server_url1.png

send_command "/sendchars http://localhost/"
send_command "/screenshot add_server_url2.png"
imagesdiff add_server_url1.png add_server_url2.png

send_command "/guikey 9 keyGUITab"
send_command "/guikey 9 keyGUITab"
send_command "/guikey 9 keyGUITab"
send_command "/guikey 9 keyGUITab"

# add new server
send_command "/guikey 13 keyGUISelect2"
send_command "/screenshot new_server.png"
imagesdiff add_server_url2.png new_server.png

# press about button
send_command "/sendmousekey 500 20 1"
send_command "/screenshot about.png"
imagesdiff new_server.png about.png

# open about page
send_command "/sendmousekey 500 20 1"
send_command "/screenshot about.png"

# open about manaverse page
send_command "/sendmousekey 300 ${ABOUTYOFFSET} 1"
send_command "/screenshot about2.png"
imagesdiff about.png about2.png

# open help window again
send_command "/sendmousekey 220 200 1"
send_command "/screenshot help2.png"
imagesdiff about2.png help2.png

# other comparisions
imagesdiff downkey.png help.png
imagesdiff help.png setup.png
imagesdiff run5.png add_server_name.png
imagesdiff run5.png add_server_adress1.png
imagesdiff run5.png add_server_adress2.png
imagesdiff run5.png add_server_port.png
imagesdiff run5.png add_server_type.png
imagesdiff run5.png add_server_description1.png
imagesdiff run5.png add_server_description2.png
imagesdiff run5.png add_server_url1.png
imagesdiff run5.png add_server_url2.png
imagesdiff run5.png new_server.png
imagesdiff run5.png about.png
imagesdiff run5.png about2.png
imagesdiff run5.png help2.png

imagesdiff run4.png add_server_name.png
imagesdiff run4.png add_server_adress1.png
imagesdiff run4.png add_server_adress2.png
imagesdiff run4.png add_server_port.png
imagesdiff run4.png add_server_type.png
imagesdiff run4.png add_server_description1.png
imagesdiff run4.png add_server_description2.png
imagesdiff run4.png add_server_url1.png
imagesdiff run4.png add_server_url2.png
imagesdiff run4.png new_server.png
imagesdiff run4.png about.png
imagesdiff run4.png about2.png
imagesdiff run4.png help2.png

imagesdiff help.png about.png
imagesdiff help.png help2.png

# final delay
sleep 5

kill_client
print_run_log
check_for_asserts

exit 0
