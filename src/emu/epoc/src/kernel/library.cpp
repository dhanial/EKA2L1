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

#include <common/cvt.h>

#include <epoc/kernel/library.h>
#include <epoc/loader/e32img.h>
#include <epoc/loader/romimage.h>

#include <epoc/kernel/libmanager.h>

#include <epoc/kernel.h>
#include <epoc/mem.h>

#include <common/log.h>
#include <common/chunkyseri.h>

namespace eka2l1 {
    namespace kernel {
        library::library(kernel_system *kern, const std::string &lib_name, loader::romimg_ptr img)
            : kernel_obj(kern, lib_name, access_type::global_access) {
            obj_type = object_type::library;
            lib_type = rom_img_library;

            state = library_state::loaded;

            rom_img = img;
        }

        library::library(kernel_system *kern, const std::string &lib_name, loader::e32img_ptr img)
            : kernel_obj(kern, lib_name, access_type::global_access) {
            obj_type = object_type::library;
            lib_type = e32_img_library;

            state = library_state::loaded;

            e32_img = img;
        }

        std::optional<uint32_t> library::get_ordinal_address(const uint8_t idx) {
            if (lib_type == e32_img_library) {
                if (e32_img->ed.syms.size() < idx) {
                    return std::optional<uint32_t>{};
                }

                LOG_TRACE("{}-{} (0x{:x}) 0x{:x}", obj_name, idx, e32_img->rt_code_addr, e32_img->rt_code_addr + 
                    e32_img->ed.syms[idx - 1] - e32_img->header.code_base);

                return e32_img->rt_code_addr + e32_img->ed.syms[idx - 1] - e32_img->header.code_base;
            }

            if (rom_img->exports.size() < idx) {
                return std::optional<uint32_t>{};
            }

            return rom_img->exports[idx - 1];
        }

        std::vector<uint32_t> library::attach() {
            if (state != library_state::attaching) {
                state = library_state::attaching;

                if (lib_type == e32_img_library) {
                    std::vector<uint32_t> entries;
                    uint32_t main = e32_img->rt_code_addr;

                    std::function<void(loader::e32img_ptr)> do_entries_query = [&](loader::e32img_ptr img) -> void {
                        for (const auto &name : img->dll_names) {
                            loader::e32img_ptr e32img = kern->get_lib_manager()->load_e32img(
                                common::utf8_to_ucs2(name));

                            if (!e32img) {
                                loader::romimg_ptr romimg = kern->get_lib_manager()->load_romimg(
                                    common::utf8_to_ucs2(name));

                                if (romimg) {
                                    entries.push_back(romimg->header.entry_point);
                                }
                            } else {
                                entries.push_back(e32img->rt_code_addr);
                                do_entries_query(e32img);
                            }
                        }

                        entries.push_back(img->rt_code_addr);
                    };

                    do_entries_query(e32_img);

                    return entries;
                }

                std::vector<uint32_t> entries;

                // Fetch all sub dll
                struct dll_ref_table {
                    uint16_t flags;
                    uint16_t num_entries;
                    uint32_t rom_img_headers_ref[25];
                };

                std::function<void(loader::rom_image_header *)> fetch_romimg_entries = [&](loader::rom_image_header *header) {
                    if (header->dll_ref_table_address != 0) {
                        dll_ref_table *ref_table = eka2l1::ptr<dll_ref_table>(header->dll_ref_table_address).get(kern->get_memory_system());

                        for (uint16_t i = 0; i < ref_table->num_entries; i++) {
                            fetch_romimg_entries(eka2l1::ptr<loader::rom_image_header>(ref_table->rom_img_headers_ref[i])
                                                     .get(kern->get_memory_system()));
                        }
                    }

                    entries.push_back(header->entry_point);
                };

                fetch_romimg_entries(&rom_img->header);

                return entries;
            }

            return std::vector<uint32_t>{};
        }

        bool library::attached() {
            if (state == library_state::attached || state != library_state::attaching) {
                return false;
            }

            state = library_state::attached;

            return true;
        }

        void library::do_state(common::chunkyseri &seri) {
            seri.absorb(state);
            seri.absorb(lib_type);

            if (lib_type == e32_img_library) {
                e32_img = kern->get_lib_manager()->load_e32img(
                    common::utf8_to_ucs2(obj_name));
            } else {
                rom_img = kern->get_lib_manager()->load_romimg(
                    common::utf8_to_ucs2(obj_name));
            }
        }
    }
}