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

#include <core/core_kernel.h>
#include <core/core_mem.h>
#include <core/ptr.h>

#include <core/arm/jit_factory.h>

#include <core/core_timing.h>
#include <core/disasm/disasm.h>
#include <core/gdbstub/gdbstub.h>
#include <core/hle/libmanager.h>
#include <core/loader/rom.h>
#include <core/manager/manager.h>
#include <core/vfs.h>

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <tuple>

namespace eka2l1 {
    namespace epoc {
        struct hal;
    }

    namespace drivers {
        class graphics_driver_client;
        class driver;

        using driver_instance = std::shared_ptr<driver>;
    }

    class debugger_base;
    using debugger_ptr = std::shared_ptr<debugger_base>;

    using hal_ptr = std::shared_ptr<epoc::hal>;
    using graphics_driver_client_ptr = std::shared_ptr<drivers::graphics_driver_client>;

    /*! A system instance, where all the magic happens. 
     *
     * Represents the Symbian system. You can switch the system version dynamiclly.
    */
    class system {
        //! Global lock mutex for system.
        std::mutex mut;

        //! The library manager.
        hle::lib_manager hlelibmngr;

        //! The cpu
        arm::jitter cpu;

        //! Jit type.
        arm::jitter_arm_type jit_type;

        graphics_driver_client_ptr gdriver_client;

        //! The memory system.
        memory_system mem;

        //! The kernel system.
        kernel_system kern;

        //! The timing system.
        timing_system timing;

        //! Manager system.
        /*!
            This manages all apps and dlls.
        */
        manager_system mngr;

        //! The IO system
        io_system io;

        //! Disassmebly helper.
        disasm asmdis;

        gdbstub gdb_stub;

        debugger_ptr debugger;

        //! The ROM
        /*! This is the information parsed
         * from the ROM, used as utility.
        */
        loader::rom romf;

        bool reschedule_pending;

        epocver ver = epocver::epoc9;
        bool exit = false;

        std::unordered_map<std::string, bool> bool_configs;
        std::unordered_map<uint32_t, hal_ptr> hals;

        std::vector<std::string> startup_apps;
        std::vector<std::string> force_load_libs;

        bool startup_inited = false;

        /*! \brief Load the core configs.
        */
        void load_configs();

        /*! \brief Save the core configs. */
        void write_configs();

        bool save_snapshot_processes(const std::string &path,
            const std::vector<uint32_t> &inclue_uids);

    public:
        bool get_bool_config(const std::string name) {
            return bool_configs[name];
        }

        system(debugger_ptr debugger, drivers::driver_instance graphics_driver,
            arm::jitter_arm_type jit_type = arm::jitter_arm_type::unicorn);

        void set_graphics_driver(drivers::driver_instance graphics_driver);

        void set_debugger(debugger_ptr new_debugger) {
            debugger = std::move(new_debugger);
        }

        void set_symbian_version_use(const epocver ever) {
            kern.set_epoc_version(ever);
            io.set_epoc_version(ever);
        }

        void set_jit_type(const arm::jitter_arm_type type) {
            jit_type = type;
        }

        loader::rom &get_rom_info() {
            return romf;
        }

        epocver get_symbian_version_use() const {
            return kern.get_epoc_version();
        }

        void prepare_reschedule() {
            cpu->prepare_rescheduling();
            reschedule_pending = true;
        }

        void init();
        uint32_t load(uint32_t id);
        int loop();
        void shutdown();

        /*!\brief Snapshot is a way to save the state of the system.
         *
         * Snapshot can be used for fast startup. Here, in EKA2L1,
         * after the first UI process runs well, the state of all
         * processes will be saved and load in the next running
         * session.
         *
         * The snapshot will save all of the following:
         * - The EPOC version
         * - All kernel objects (semaphore, mutex, etc...)
         * - Global memory data that is committed.
         * - Local data for each process
         * - Thread state, current running thread and process
         *
         * The following will not be saved:
         * - The ROM content.
         * - Page that is marked as free.
         *
         * \params name The path to save the snapshot. Note that the snapshot
         *              can be really large.
         *
         * \returns     True if successfully save the snapshot
         */
        bool save_snapshot(const std::string &name);
        bool save_snapshot_exclude_current_process(const std::string &name);

        bool load_snapshot(const std::string &name);

        manager_system *get_manager_system() {
            return &mngr;
        }

        memory_system *get_memory_system() {
            return &mem;
        }

        kernel_system *get_kernel_system() {
            return &kern;
        }

        hle::lib_manager *get_lib_manager() {
            return &hlelibmngr;
        }

        io_system *get_io_system() {
            return &io;
        }

        timing_system *get_timing_system() {
            return &timing;
        }

        disasm *get_disasm() {
            return &asmdis;
        }

        gdbstub *get_gdb_stub() {
            return &gdb_stub;
        }

        graphics_driver_client_ptr get_graphic_driver_client() {
            return gdriver_client;
        }

        arm::jitter &get_cpu() {
            return cpu;
        }

        void mount(drive_number drv, const drive_media media, std::string path,
            const io_attrib attrib = io_attrib::none);

        void reset();

        /*! \brief Install an Z drive repackage. 
         *
         * \returns True on success.
         */
        bool install_rpkg(const std::string &path);
        void load_scripts();

        /*! \brief Install a SIS/SISX. */
        bool install_package(std::u16string path, uint8_t drv);
        bool load_rom(const std::string &path);

        void request_exit();
        bool should_exit() const {
            return exit;
        }

        size_t total_app() {
            return mngr.get_package_manager()->app_count();
        }

        std::vector<manager::app_info> app_infos() {
            return mngr.get_package_manager()->get_apps_info();
        }

        void add_new_hal(uint32_t hal_cagetory, hal_ptr hal_com);
        hal_ptr get_hal(uint32_t cagetory);
    };
}
