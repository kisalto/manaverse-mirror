/*
 *  The ManaVerse Client
 *  Copyright (C) 2011-2020  The ManaPlus Developers
 *  Copyright (C) 2020-2025  The ManaVerse Developers
 *
 *  This file is part of The ManaVerse Client.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gui/windows/maileditwindow.h"

#include "settings.h"

#include "being/playerinfo.h"

#include "enums/gui/layouttype.h"

#include "gui/windows/inventorywindow.h"
#include "gui/windows/itemamountwindow.h"

#include "gui/widgets/button.h"
#include "gui/widgets/inttextfield.h"
#include "gui/widgets/itemcontainer.h"
#include "gui/widgets/label.h"
#include "gui/widgets/layout.h"
#include "gui/widgets/scrollarea.h"

#include "resources/item/item.h"

#include "net/mail2handler.h"
#include "net/mailhandler.h"

#include "utils/delete2.h"
#include "utils/gettext.h"

#include <climits>

#include "debug.h"

MailEditWindow *mailEditWindow = nullptr;

MailEditWindow::MailEditWindow() :
    // TRANSLATORS: mail edit window name
    Window(_("Edit mail"), Modal_false, nullptr, "mailedit.xml"),
    ActionListener(),
    FocusListener(),
    // TRANSLATORS: mail edit window button
    mSendButton(new Button(this, _("Send"), "send", BUTTON_SKIN, this)),
    // TRANSLATORS: mail edit window button
    mCloseButton(new Button(this, _("Close"), "close", BUTTON_SKIN, this)),
    // TRANSLATORS: mail edit window button
    mAddButton(new Button(this, _("Add"), "add", BUTTON_SKIN, this)),
    // TRANSLATORS: mail edit window label
    mToLabel(new Label(this, _("To:"))),
    // TRANSLATORS: mail edit window label
    mSubjectLabel(new Label(this, _("Subject:"))),
    // TRANSLATORS: mail edit window label
    mMoneyLabel(new Label(this, _("Money:"))),
    // TRANSLATORS: mail edit window label
    mItemLabel(new Label(this, _("Items:"))),
    // TRANSLATORS: mail edit window label
    mMessageLabel(new Label(this, _("Message:"))),
    mToField(new TextField(this, std::string(), LoseFocusOnTab_true,
        nullptr, std::string(), false)),
    mSubjectField(new TextField(this, std::string(), LoseFocusOnTab_true,
        nullptr, std::string(), false)),
    mMoneyField(new IntTextField(this, 0, 0,
        settings.enableNewMailSystem ? INT_MAX : 10000000, Enable_true, 0)),
    mMessageField(new TextField(this, std::string(), LoseFocusOnTab_true,
        nullptr, std::string(), false)),
    mInventory(new Inventory(InventoryType::MailEdit,
        settings.enableNewMailSystem ? -1 : 1)),
    mItemContainer(new ItemContainer(this, mInventory, 100000,
        ShowEmptyRows_false, ForceQuantity_false)),
    mItemScrollArea(new ScrollArea(this, mItemContainer,
        fromBool(getOptionBool("showitemsbackground", false), Opaque),
        "mailedit_listbackground.xml")),
    mUseMail2(settings.enableNewMailSystem)
{
    setWindowName("MailEdit");
    setCloseButton(true);
    setResizable(true);
    setCloseButton(true);
    setSaveVisible(false);
    setStickyButtonLock(true);
    setVisible(Visible_true);

    mToField->setWidth(100);
    mSubjectField->setWidth(100);
    mMessageField->setWidth(100);
    // 1x3 items (current ML limit) times some padding
    // + a handful of pixels to cover scrollbar/padding
    mItemScrollArea->setHeight(43); // same as itemcontainer default
    mItemScrollArea->setWidth(3 * (32 + 2) + 5 + 10);
    mItemScrollArea->setHorizontalScrollPolicy(ScrollArea::SHOW_NEVER);
    mToField->addFocusListener(this);

    place(0, 0, mToLabel);
    place(1, 0, mToField, 2, 1);
    place(0, 1, mSubjectLabel);
    place(1, 1, mSubjectField, 2, 1);
    place(0, 2, mMoneyLabel);
    place(1, 2, mMoneyField, 2, 1);
    place(0, 3, mItemLabel);
    place(1, 3, mItemScrollArea, 1, 2);
    place(2, 4, mAddButton);

    place(0, 5, mMessageLabel);
    place(1, 5, mMessageField, 2, 1);
    place(0, 6, mSendButton);
    place(2, 6, mCloseButton);

    Layout &layout = getLayout();
    // mid column shall eat all remaining available window space.
    layout.setColWidth(1, LayoutType::SET);
    layout.setRowHeight(3, LayoutType::SET);

    // Enforce a minimum size.
    int width = 0;
    int height = 0;
    layout.reflow(width, height);
    setContentSize(width, height);

    setMinWidth(getWidth());
    setMinHeight(getHeight());
    setDefaultSize(getWidth(), getHeight(), ImagePosition::CENTER, 0, 0);
    center();

    loadWindowState();
    if (mUseMail2)
        mSendButton->setEnabled(false);
    enableVisibleSound(true);
}

MailEditWindow::~MailEditWindow()
{
    mailEditWindow = nullptr;
    delete2(mInventory)
}

void MailEditWindow::action(const ActionEvent &event)
{
    const std::string &eventId = event.getId();
    if (eventId == "close")
    {
        close();
    }
    else if (eventId == "send")
    {
        sendMail();
    }
    else if (eventId == "add")
    {
        Item *const item = inventoryWindow->getSelectedItem();

        if (item == nullptr)
            return;

        ItemAmountWindow::showWindow(ItemAmountWindowUsage::MailAdd,
            this,
            item,
            0,
            0);
    }
}

void MailEditWindow::addItem(const Item *const item,
                             const int amount)
{
    if (item == nullptr)
        return;
    mInventory->addItem(item->getId(),
        item->getType(),
        amount,
        item->getRefine(),
        item->getColor(),
        item->getIdentified(),
        item->getDamaged(),
        item->getFavorite(),
        Equipm_false,
        Equipped_false);
}

void MailEditWindow::setSubject(const std::string &str)
{
    mSubjectField->setText(str);
}

void MailEditWindow::setTo(const std::string &str)
{
    mToField->setText(str);
    mSendButton->setEnabled(true);
}

void MailEditWindow::setMessage(const std::string &str)
{
    mMessageField->setText(str);
}

void MailEditWindow::close()
{
    if (mUseMail2)
        mail2Handler->cancelWriteMail();
    mailEditWindow = nullptr;
    scheduleDelete();
}

Inventory *MailEditWindow::getInventory() const
{
    return mInventory;
}

void MailEditWindow::sendMail()
{
    const int money = mMoneyField->getValue();
    std::string subject = mSubjectField->getText();
    if (subject.empty())
    {
        // TRANSLATORS: empty mail message subject
        subject.append(_("empty subject"));
    }
    if (mUseMail2)
    {
        mail2Handler->sendMail(mToField->getText(),
            subject,
            mMessageField->getText(),
            money);
    }
    else
    {
        if (money != 0)
            mailHandler->setAttachMoney(money);
        const Item *const tempItem = mInventory->getItem(0);
        if (tempItem != nullptr)
        {
            const Inventory *const inv = PlayerInfo::getInventory();
            if (inv != nullptr)
            {
                const Item *const item = inv->findItem(
                    tempItem->getId(), ItemColor_one);
                if (item != nullptr)
                {
                    mailHandler->setAttach(item->getInvIndex(),
                        tempItem->getQuantity());
                }
            }
        }

        mailHandler->send(mToField->getText(),
            subject,
            mMessageField->getText());
    }
}

void MailEditWindow::updateItems()
{
    mItemContainer->updateMatrix();
}

void MailEditWindow::focusLost(const Event &event)
{
    if (!mUseMail2)
        return;

    if (event.getSource() == mToField)
    {
        const std::string to = mToField->getText();
        if (to != mail2Handler->getCheckedName())
        {
            mail2Handler->queueCheckName(MailQueueType::ValidateTo,
                to,
                std::string(),
                std::string(),
                0);
            mSendButton->setEnabled(false);
        }
        else
        {
            mSendButton->setEnabled(true);
        }
    }
}

void MailEditWindow::validatedTo()
{
    mSendButton->setEnabled(true);
}
