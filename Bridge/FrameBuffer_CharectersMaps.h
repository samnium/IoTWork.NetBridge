////////////////////////////////////////////////////////////////////////////
//
//  This file is part of NetBridgeLib
//
//  The MIT License (MIT)
//  Copyright (c) 2017 Gianluca Masone
//
//  Permission is hereby granted, free of charge, to any person obtaining
//  a copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//   Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//
////////////////////////////////////////////////////////////////////////////

#ifndef _NETBRIDGE_FrameBuffer_CharectersMaps_H
#define	_NETBRIDGE_FrameBuffer_CharectersMaps_H

#define _FB_MAP_BK_COLOR    0x0000
#define _FB_MAP_FG_COLOR    0xF800

#define _AA_ _FB_MAP_BK_COLOR
#define _xx_ _FB_MAP_FG_COLOR

uint16_t _fb_char_map_8x8_H[8][8] = {
    { _AA_, _xx_, _xx_, _AA_, _AA_, _xx_, _xx_ , _AA_},
    { _AA_, _xx_, _xx_, _AA_, _AA_, _xx_, _xx_ , _AA_},
    { _AA_, _xx_, _xx_, _AA_, _AA_, _xx_, _xx_ , _AA_},
    { _AA_, _xx_, _xx_, _xx_, _xx_, _xx_, _xx_ , _AA_},
    { _AA_, _xx_, _xx_, _xx_, _xx_, _xx_, _xx_ , _AA_},
    { _AA_, _xx_, _xx_, _AA_, _AA_, _xx_, _xx_ , _AA_},
    { _AA_, _xx_, _xx_, _AA_, _AA_, _xx_, _xx_ , _AA_},
    { _AA_, _xx_, _xx_, _AA_, _AA_, _xx_, _xx_ , _AA_},
};

uint16_t _fb_char_map_8x8_P[8][8] = {
    { _AA_, _xx_, _xx_, _xx_, _xx_, _AA_, _AA_ , _AA_},
    { _AA_, _xx_, _xx_, _AA_, _AA_, _xx_, _AA_ , _AA_},
    { _AA_, _xx_, _xx_, _AA_, _AA_, _xx_, _AA_ , _AA_},
    { _AA_, _xx_, _xx_, _AA_, _AA_, _xx_, _AA_ , _AA_},
    { _AA_, _xx_, _xx_, _xx_, _xx_, _AA_, _AA_ , _AA_},
    { _AA_, _xx_, _xx_, _AA_, _AA_, _AA_, _AA_ , _AA_},
    { _AA_, _xx_, _xx_, _AA_, _AA_, _AA_, _AA_ , _AA_},
    { _AA_, _xx_, _xx_, _AA_, _AA_, _AA_, _AA_ , _AA_},
};

uint16_t _fb_char_map_8x8_T[8][8] = {
    { _AA_, _xx_, _xx_, _xx_, _xx_, _xx_, _xx_ , _AA_},
    { _AA_, _xx_, _xx_, _xx_, _xx_, _xx_, _xx_ , _AA_},
    { _AA_, _AA_, _AA_, _xx_, _xx_, _AA_, _AA_ , _AA_},
    { _AA_, _AA_, _AA_, _xx_, _xx_, _AA_, _AA_ , _AA_},
    { _AA_, _AA_, _AA_, _xx_, _xx_, _AA_, _AA_ , _AA_},
    { _AA_, _AA_, _AA_, _xx_, _xx_, _AA_, _AA_ , _AA_},
    { _AA_, _AA_, _AA_, _xx_, _xx_, _AA_, _AA_ , _AA_},
    { _AA_, _AA_, _AA_, _xx_, _xx_, _AA_, _AA_ , _AA_},
};


#endif