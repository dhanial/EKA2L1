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

#include <common/algorithm.h>
#include <common/log.h>

#include <epoc/loader/romimage.h>
#include <epoc/vfs.h>
#include <epoc/ptr.h>

#include <algorithm>
#include <cstdio>
#include <set>

namespace eka2l1 {
    namespace loader {
        uint32_t rom_to_offset_n(address romstart, address off) {
            return off - romstart;
        }

        // Unstable
        std::optional<romimg> parse_romimg(symfile &file, memory_system *mem) {
            romimg img;
            file->read_file(&img, 1, sizeof(rom_image_header));

            ptr<uint32_t> export_off(img.header.export_dir_address);

            for (int32_t i = 0; i < img.header.export_dir_count; i++) {
                auto export_addr = *export_off.get(mem);
                img.exports.push_back(export_addr);

                export_off += 4;
            }

            return img;
        }
    }
}