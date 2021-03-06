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

#include <drivers/driver.h>
#include <drivers/itc.h>

#include <drivers/graphics/graphics.h>

namespace eka2l1::drivers {
    bool driver_client::send_opcode(const int opcode, itc_context &ctx) {
        if (already_locked) {
            driver->request_queue.push_unsafe({ opcode, ctx });
        } else {
            driver->request_queue.push({ opcode, ctx });
        }

        return true;
    }

    void driver_client::lock_driver_from_process()  {
        driver->request_queue.lock.lock();
        already_locked = true;
    }

    void driver_client::unlock_driver_from_process() {
        driver->request_queue.lock.unlock();
        already_locked = false;
    }

    void driver_client::sync_with_driver() {
        std::unique_lock<std::mutex> ulock(lock);
        driver->cond.wait(ulock);
    }

    driver_client::driver_client(driver_instance driver) 
        : driver(driver) {

    }

    graphics_driver_client::graphics_driver_client(driver_instance driver)
        : driver_client(driver) {

    }

    /*! \brief Get the screen size in pixels
    */
    vec2 graphics_driver_client::screen_size() {
        // Synchronize call should directly use the client
        std::shared_ptr<graphics_driver> gdriver = 
            std::reinterpret_pointer_cast<graphics_driver>(driver);

        return gdriver->get_screen_size();
    }

    void graphics_driver_client::set_screen_size(eka2l1::vec2 &s) {
        itc_context context;
        context.push(s);

        send_opcode(graphics_driver_resize_screen, context);
    }
    
    void graphics_driver_client::invalidate(eka2l1::rect &rect) {
        itc_context context;
        context.push(rect);

        send_opcode(graphics_driver_invalidate, context);
    }

    void graphics_driver_client::end_invalidate() {
        itc_context context;
        send_opcode(graphics_driver_end_invalidate, context);
    }

    /*! \brief Clear the screen with color.
        \params color A RGBA vector 4 color
    */
    void graphics_driver_client::clear(vecx<int, 4> color) {
        itc_context context;
        context.push(color[0]);
        context.push(color[1]);
        context.push(color[2]);
        context.push(color[3]);

        send_opcode(graphics_driver_clear, context);
    }

    void graphics_driver_client::draw_text(eka2l1::rect rect, const std::string &str) {
        itc_context context;
        context.push(rect);
        context.push_string(str);

        send_opcode(graphics_driver_draw_text_box, context);
    }
}