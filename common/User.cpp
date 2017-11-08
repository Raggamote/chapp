// The MIT License (MIT)
//
// Copyright (c) 2017 Viktor Oreshkin <imselfish@stek29.rocks>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "User.hpp"

#include "Group.hpp"
#include "Placeholders.hpp"
#include <utility>

namespace Chapp {

    User::User(int32_t uid, const string& username)
            : User(uid, username, gen_rand_phash()) {};

    User::User(int32_t uid, string username, phash hash)
            : id(uid)
            , username(std::move(username))
            , pass_hash(hash)
            , last_activity(time(nullptr)) // Might this become a bottleneck?
    {};

    User::~User() {
        for (auto &group_id : joined_groups) {
            // TODO(stek29): Avoid constructing groups just to delete user?
            auto group = GroupFactory::getInstance().by_id(group_id);
            if (group == nullptr) {
                continue; // WTF?!
            }

            // TODO(stek29): When socket is added, avoid notifying it here
            group->leave(id);
        }
    }

    bool User::deliver_message(Message msg) {
        // TODO(stek29): send to socket
        (void) msg;
        return true;
    }

    bool User::invite(int32_t inviter_id, const GroupInvite& invite) {
        auto gid = invite.group.id;

        auto it = invites_by_gid.find(gid);

        if (it != invites_by_gid.end()) {
            return false; // already invited
        }

        invites_by_gid.insert(std::make_pair(gid, invite));

        // TODO(stek29): notify socket
        (void) inviter_id;

        return true;
    };

    bool User::add_to_group(const Group &group) {
        auto insert_pair = joined_groups.insert(group.id);
        if (!insert_pair.second) {
            return false; // Not inserted
        }

        // Database.addUserToGroup(id, group.id);
        // TODO(stek29): notify socket
        (void)0; // To disable CLion simplify
        return true;
    }

    bool User::remove_from_group(const Group &group) {
        auto removed_cnt = joined_groups.erase(group.id);
        if (removed_cnt == 0) {
            return false; // Not erased
        }

        // Database.removeUserFromGroup(id, group.id);

        (void)0; // To disable CLion simplify
        return true;
    }

}  // namespace Chapp
