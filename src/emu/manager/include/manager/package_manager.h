/*
 * Copyright (c) 2018 EKA2L1 Team.
 * 
 * This file is part of EKA2L1 project 
 * (see bentokun.github.com/EKA2L1).
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <common/types.h>

#include <map>
#include <string>
#include <optional>
#include <vector>

namespace eka2l1 {
    class io_system;

    namespace loader {
        struct sis_controller;
    }

	/*! \brief Managing apps. */
    namespace manager {
        using uid = uint32_t;

        struct app_info {
            std::u16string name;
            std::u16string vendor_name;
            std::u16string executable_name;
            drive_number drive;

            uid id;
            epocver ver;
        };

        // A package manager, serves for managing apps
        // and implements part of HAL
        class package_manager {
            std::map<uid, app_info> apps;

            bool load_sdb_yaml(const std::string &path);
            bool write_sdb_yaml(const std::string &path);

            io_system *io;

            bool install_controller(loader::sis_controller *ctrl, drive_number drv);

        public:
            package_manager() = default;
            package_manager(io_system *io)
                : io(io) { load_sdb_yaml("apps_registry.yml"); }

            bool installed(uid app_uid);

            std::size_t app_count() {
                return apps.size();
            }

            std::vector<app_info> get_apps_info();

            std::u16string app_name(uid app_uid);
            std::optional<app_info> info(uid app_uid);

            bool install_package(const std::u16string &path, drive_number drive);
            bool uninstall_package(uid app_uid);

            std::string get_app_executable_path(uint32_t uid);
            std::string get_app_name(uint32_t uid);
        };
    }
}