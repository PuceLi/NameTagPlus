#include "Entry.h"

#include <ll/api/mod/RegisterHelper.h>
#include <ll/api/memory/Hook.h>
#include <ll/api/io/Logger.h>

#include <mc/world/actor/player/Player.h>
#include <mc/world/item/ItemStack.h>
#include <mc/world/actor/Actor.h>
#include <mc/world/level/Level.h>
#include <mc/world/inventory/transaction/ItemUseOnActorInventoryTransaction.h>
#include <mc/world/inventory/transaction/InventoryTransactionError.h>
#include <mc/world/actor/player/PlayerInventory.h>

using namespace ll::memory;

LL_AUTO_TYPE_INSTANCE_HOOK(
    ForceNameTagHook,
    ll::memory::HookPriority::Normal,
    ItemUseOnActorInventoryTransaction,
    &ItemUseOnActorInventoryTransaction::$handle,
    InventoryTransactionError,
    Player& player,
    bool isSenderAuthority
) {
    auto& actionType = (ItemUseOnActorInventoryTransaction::ActionType&)this->mActionType;
    if (actionType == ItemUseOnActorInventoryTransaction::ActionType::Attack) {
         return origin(player, isSenderAuthority);
    }

    const auto& item = player.getSelectedItem();

    if (!item.isNull() && item.getTypeName() == "minecraft:name_tag" && item.hasCustomHoverName()) {
        
        auto& targetId = (ActorRuntimeID&)this->mRuntimeId;
        
        auto target = player.getLevel().getRuntimeEntity(targetId, false);
        
        if (target) {
            std::string newName = item.getCustomName();
            
            target->setNameTag(newName);
            target->setNameTagVisible(true);
            target->setPersistent(); 

            if (!player.isCreative()) {
                ItemStack handItem = item;
                
                handItem.remove(1);
                player.setCarriedItem(handItem);
                player.sendInventory(true);
            }
            return (InventoryTransactionError)0; 
        }
    }
    return origin(player, isSenderAuthority);
}

namespace nametag {

NameTagPlus& NameTagPlus::getInstance() {
    static NameTagPlus instance;
    return instance;
}

bool NameTagPlus::load() {
    getSelf().getLogger().info("NameTagPlus 加载中......");
    getSelf().getLogger().info("Author: PuceLi");
    return true;
}

bool NameTagPlus::enable() {
    getSelf().getLogger().info("NameTagPlus 已启用");
    return true;
}

bool NameTagPlus::disable() {
    getSelf().getLogger().info("NameTagPlus 已禁用");
    return true;
}

} // namespace nametag

LL_REGISTER_MOD(nametag::NameTagPlus, nametag::NameTagPlus::getInstance());