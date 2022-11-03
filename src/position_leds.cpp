/***************************************************************************
**                                                                        **
**  FlySight firmware                                                     **
**  Copyright 2018 Michael Cooper, Tom van Dijck                          **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see <http://www.gnu.org/licenses/>. **
**                                                                        **
****************************************************************************
**  Contact: Michael Cooper                                               **
**  Website: http://flysight.ca/                                          **
****************************************************************************/

#ifdef __AVR__
#    include "position_leds.h"
#    include <avr/interrupt.h>
#    include <avr/io.h>
#else
#    include "flysight/fw/position_leds.h"
#endif

namespace flysight {
namespace fw {
namespace detail {

#ifdef __AVR__
void display_8mhz(const size_t num_bytes, const uint8_t* const pixels, volatile uint8_t* port_ptr,
                  const uint8_t bit) {
    const uint8_t pin_mask = (1 << bit);

    volatile uint16_t i = num_bytes;      // Loop counter
    volatile uint8_t const* ptr = pixels; // Pointer to next byte
    volatile uint8_t b = *ptr++;          // Current byte value
    volatile uint8_t hi;                  // PORT w/output bit set high
    volatile uint8_t lo;                  // PORT w/output bit set low

    volatile uint8_t n1, n2 = 0; // First, next bits out

    hi = (*port_ptr) | pin_mask;
    lo = (*port_ptr) & ~pin_mask;
    n1 = lo;
    if (b & 0x80) {
        n1 = hi;
    }

    // 11 instruction clocks per bit: HHHxxxxLLLL
    // OUT instructions:              ^  ^   ^   (T=0,3,7,10)

    // 0x28 -> 0x14: USB General
    // 0x2c -> 0x16: USB Endpoint/Pipe
    //
    // 0x50 -> 0x28: TIMER1 OVF
    // 0x54 -> 0x2a: TIMER0 COMPA
    //
    // 0x64 -> 0x32: USART1 RX
    // 0x68 -> 0x34: USART1 UDRE
    //
    // 0x80 -> 0x40: TIMER3 COMPA

#    if 1
#        define SERVICE_INTERRUPTS "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
#        define DISABLE_INTERRUPTS "cli\n"
#        define ENABLE_INTERRUPTS "sei\n"
#    else
#        define SERVICE_INTERRUPTS ""
#        define DISABLE_INTERRUPTS ""
#        define ENABLE_INTERRUPTS ""
#    endif

//#define PER_BIT_INTERRUPTS ENABLE_INTERRUPTS SERVICE_INTERRUPTS DISABLE_INTERRUPTS
#    define PER_BIT_INTERRUPTS ""

    asm volatile("loop:\n" SERVICE_INTERRUPTS DISABLE_INTERRUPTS
                 // Bit 7
                 "st %a[port], %[hi]\n" // 0, 1     : PORT = hi
                 "mov %[n2], %[lo]\n"   // 2        : n2 = lo
                 "st %a[port], %[n1]\n" // 3, 4     : PORT = n1
                 "sbrc %[byte], 6\n"    // 5        : if (b & 0x40)
                 "mov %[n2], %[hi]\n"   // 6        :   n2 = hi
                 "st %a[port], %[lo]\n" // 7, 8     : PORT = lo
                 PER_BIT_INTERRUPTS
                 // Bit 6
                 "st %a[port], %[hi]\n" // 0, 1     : PORT = hi
                 "mov %[n1], %[lo]\n"   // 2        : n1 = lo
                 "st %a[port], %[n2]\n" // 3, 4     : PORT = n2
                 "sbrc %[byte], 5\n"    // 5        : if (b & 0x20)
                 "mov %[n1], %[hi]\n"   // 6        :   n1 = hi
                 "st %a[port], %[lo]\n" // 7, 8     : PORT = lo
                 PER_BIT_INTERRUPTS
                 // Bit 5
                 "st %a[port], %[hi]\n" // 0, 1     : PORT = hi
                 "mov %[n2], %[lo]\n"   // 2        : n2 = lo
                 "st %a[port], %[n1]\n" // 3, 4     : PORT = n1
                 "sbrc %[byte], 4\n"    // 5        : if (b & 0x10)
                 "mov %[n2], %[hi]\n"   // 6        :   n2 = hi
                 "st %a[port], %[lo]\n" // 7, 8     : PORT = lo
                 PER_BIT_INTERRUPTS
                 // Bit 4
                 "st %a[port], %[hi]\n" // 0, 1     : PORT = hi
                 "mov %[n1], %[lo]\n"   // 2        : n1 = lo
                 "st %a[port], %[n2]\n" // 3, 4     : PORT = n2
                 "sbrc %[byte], 3\n"    // 5        : if (b & 0x08)
                 "mov %[n1], %[hi]\n"   // 6        :   n1 = hi
                 "st %a[port], %[lo]\n" // 7, 8     : PORT = lo
                 PER_BIT_INTERRUPTS
                 // Bit 3
                 "st %a[port], %[hi]\n" // 0, 1     : PORT = hi
                 "mov %[n2], %[lo]\n"   // 2        : n2 = lo
                 "st %a[port], %[n1]\n" // 3, 4     : PORT = n1
                 "sbrc %[byte], 2\n"    // 5        : if (b & 0x04)
                 "mov %[n2], %[hi]\n"   // 6        :   n2 = hi
                 "st %a[port], %[lo]\n" // 7, 8     : PORT = lo
                 PER_BIT_INTERRUPTS
                 // Bit 2
                 "st %a[port], %[hi]\n" // 0, 1     : PORT = hi
                 "mov %[n1], %[lo]\n"   // 2        : n1 = lo
                 "st %a[port], %[n2]\n" // 3, 4     : PORT = n2
                 "sbrc %[byte], 1\n"    // 5        : if (b & 0x02)
                 "mov %[n1], %[hi]\n"   // 6        :   n1 = hi
                 "st %a[port], %[lo]\n" // 7, 8     : PORT = lo
                 PER_BIT_INTERRUPTS
                 // Bit 1
                 "st %a[port], %[hi]\n" // 0, 1     : PORT = hi
                 "mov %[n2], %[lo]\n"   // 2        : n2 = lo
                 "st %a[port], %[n1]\n" // 3, 4     : PORT = n1
                 "sbrc %[byte], 0\n"    // 5        : if (b & 0x01)
                 "mov %[n2], %[hi]\n"   // 6        :   n2 = hi
                 "st %a[port], %[lo]\n" // 7, 8     : PORT = lo
                 PER_BIT_INTERRUPTS
                 // Bit 0
                 "st %a[port], %[hi]\n" // 0, 1     : PORT = hi
                 "mov %[n1], %[lo]\n"   // 2        : n1 = lo
                 "st %a[port], %[n2]\n" // 3        : PORT = n2
                 "rjmp .+0\n"           // 4, 5     : nop nop
                 "st %a[port], %[lo]\n" // 6, 7     : PORT = lo
                 ENABLE_INTERRUPTS
                 // loop
                 "ld %[byte] , %a[ptr]+\n" // -- 2     : b = *ptr++
                 "sbrc %[byte], 7\n"       // -- 1     : if (b & 0x80)
                 "mov %[n1], %[hi]\n"      // -- 1     :   n1 = hi
                 "sbiw %[count], 1\n"      // -- 2
                 "breq done\n"             // -- 1
                 "rjmp loop\n"             // -- 2
                 "done: nop\n"
                 : [ port ] "+e"(port_ptr), [ byte ] "+r"(b), [ n1 ] "+r"(n1), [ n2 ] "+r"(n2),
                   [ count ] "+w"(i)
                 : [ ptr ] "e"(ptr), [ hi ] "r"(hi), [ lo ] "r"(lo)
                 : "r16");

#    undef SERVICE_INTERRUPTS
#    undef DISABLE_INTERRUPTS
#    undef ENABLE_INTERRUPTS
#    undef PER_BIT_INTERRUPTS
}

#else // !defined(__AVR__)

void display_8mhz(const size_t, const uint8_t* const, volatile uint8_t*, const uint8_t) {
}

#endif // ifdef __AVR__

} // namespace detail
} // namespace fw
} // namespace flysight
