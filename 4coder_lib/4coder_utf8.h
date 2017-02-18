/*
 * Mr. 4th Dimention - Allen Webster
 *
 * 17.02.2017
 *
 * Code for converting to and from utf8 to ANSI and utf16 text encodings.
 *
 */

// TOP

#if !defined(FED_UTF8_CONVERSION_H)
#define FED_UTF8_CONVERSION_H

// 4tech_standard_preamble.h
#if !defined(FTECH_INTEGERS)
#define FTECH_INTEGERS
#include <stdint.h>
typedef int8_t i8_4tech;
typedef int16_t i16_4tech;
typedef int32_t i32_4tech;
typedef int64_t i64_4tech;

typedef uint8_t u8_4tech;
typedef uint16_t u16_4tech;
typedef uint32_t u32_4tech;
typedef uint64_t u64_4tech;

typedef u64_4tech umem_4tech;

typedef float f32_4tech;
typedef double f64_4tech;

typedef int8_t b8_4tech;
typedef int32_t b32_4tech;
#endif

#if !defined(Assert)
# define Assert(n) do{ if (!(n)) *(int*)0 = 0xA11E; }while(0)
#endif
// standard preamble end 

static u32_4tech
utf8_to_u32_unchecked(u8_4tech *buffer){
    u32_4tech result = 0;
    
    if (buffer[0] <= 0x7F){
        result = (u32_4tech)buffer[0];
    }
    else if (buffer[0] <= 0xE0){
        result = ((u32_4tech)((buffer[0])&0x1F)) << 6;
        result |= ((u32_4tech)((buffer[1])&0x3F));
    }
    else if (buffer[0] <= 0xF0){
        result = ((u32_4tech)((buffer[0])&0x0F)) << 12;
        result |= ((u32_4tech)((buffer[1])&0x3F)) << 6;
        result |= ((u32_4tech)((buffer[2])&0x3F));
    }
    else{
        result = ((u32_4tech)((buffer[0])&0x07)) << 18;
        result |= ((u32_4tech)((buffer[1])&0x3F)) << 12;
        result |= ((u32_4tech)((buffer[2])&0x3F)) << 6;
        result |= ((u32_4tech)((buffer[3])&0x3F));
    }
    
    return(result);
}

static umem_4tech
utf8_to_utf16_minimal_checking(u16_4tech *dst, umem_4tech max_wchars, u8_4tech *src, umem_4tech length, b32_4tech *error){
    u8_4tech *s = src;
    u8_4tech *s_end = s + length;
    
    u16_4tech *d = dst;
    u16_4tech *d_end = d + max_wchars;
    umem_4tech limit = length;
    
    umem_4tech needed_max = 0;
    u32_4tech advance = 1;
    
    *error = false;
    for(; s < s_end;){
        u32_4tech code_point = 0;
        u32_4tech utf8_size = 0;
        
        if (s[0] <= 0x7F){
            code_point = (u32_4tech)s[0];
            utf8_size = 1;
        }
        else if (s[0] <= 0xE0){
            if (limit <= 1){
                *error = true;
                break;
            }
            
            code_point = ((u32_4tech)((s[0])&0x1F)) << 6;
            code_point |= ((u32_4tech)((s[1])&0x3F));
            utf8_size = 2;
        }
        else if (s[0] <= 0xF0){
            if (limit <= 2){
                *error = true;
                break;
            }
            
            code_point = ((u32_4tech)((s[0])&0x0F)) << 12;
            code_point |= ((u32_4tech)((s[1])&0x3F)) << 6;
            code_point |= ((u32_4tech)((s[2])&0x3F));
            utf8_size = 3;
        }
        else{
            if (limit > 3){
                *error = true;
                break;
            }
            
            code_point = ((u32_4tech)((s[0])&0x07)) << 18;
            code_point |= ((u32_4tech)((s[1])&0x3F)) << 12;
            code_point |= ((u32_4tech)((s[2])&0x3F)) << 6;
            code_point |= ((u32_4tech)((s[3])&0x3F));
            utf8_size = 4;
        }
        
        s += utf8_size;
        limit -= utf8_size;
        
        if (code_point <= 0xD7FF || (code_point >= 0xE000 && code_point <= 0xFFFF)){
            *d = (u16_4tech)(code_point);
            d += advance;
            needed_max += 1;
        }
        else if (code_point >= 0x10000 && code_point <= 0x10FFFF){
            code_point -= 0x10000;
            
            u32_4tech high = (code_point >> 10) & 0x03FF;
            u32_4tech low = (code_point) & 0x03FF;
            
            high += 0xD800;
            low += 0xDC00;
            
            if (d + advance < d_end){
                *d = (u16_4tech)high;
                d += advance;
                *d = (u16_4tech)low;
                d += advance;
            }
            else{
                advance = 0;
            }
            
            needed_max += 2;
        }
        else{
            *error = true;
            break;
        }
        
        if (d >= d_end){
            advance = 0;
        }
    }
    
    return(needed_max);
}

static umem_4tech
utf16_to_utf8_minimal_checking(u8_4tech *dst, umem_4tech max_chars, u16_4tech *src, umem_4tech length, b32_4tech *error){
    u16_4tech *s = src;
    u16_4tech *s_end = s + max_chars;
    
    u8_4tech *d = dst;
    u8_4tech *d_end = d + length;
    umem_4tech limit = length;
    
    umem_4tech needed_max = 0;
    
    *error = false;
    
    for (; s < s_end;){
        u32_4tech code_point = 0;
        u32_4tech utf16_size = 0;
        
        if (s[0] <= 0xD7FF || (s[0] >= 0xE000 && s[0] <= 0xFFFF)){
            code_point = s[0];
            utf16_size = 1;
        }
        else{
            if (s[0] >= 0xD800 && s[0] <= 0xDBFF){
                if (limit <= 1){
                    *error = true;
                    break;
                }
                
                u32_4tech high = s[0] - 0xD800;
                u32_4tech low = s[1] - 0xDC00;
                code_point = ((high << 10) | (low)) + 0x10000;
                utf16_size = 2;
            }
            else{
                *error = true;
                break;
            }
        }
        
        s += utf16_size;
        limit -= utf16_size;
        
        u8_4tech d_fill[4];
        u32_4tech d_fill_count = 0;
        
        if (code_point <= 0x7F){
            d_fill[0] = (u8_4tech)code_point;
            d_fill_count = 1;
        }
        else if (code_point <= 0x7FF){
            d_fill[0] = (u8_4tech)(0xC0 | (code_point >> 6));
            d_fill[1] = (u8_4tech)(0x80 | (code_point & 0x3F));
            d_fill_count = 2;
        }
        else if (code_point <= 0xFFFF){
            d_fill[0] = (u8_4tech)(0xE0 | (code_point >> 12));
            d_fill[1] = (u8_4tech)(0x80 | ((code_point >> 6) & 0x3F));
            d_fill[2] = (u8_4tech)(0x80 | (code_point & 0x3F));
            d_fill_count = 3;
        }
        else if (code_point <= 0x10FFFF){
            d_fill[0] = (u8_4tech)(0xF0 | (code_point >> 18));
            d_fill[1] = (u8_4tech)(0x80 | ((code_point >> 12) & 0x3F));
            d_fill[2] = (u8_4tech)(0x80 | ((code_point >> 6) & 0x3F));
            d_fill[3] = (u8_4tech)(0x80 | (code_point & 0x3F));
            d_fill_count = 4;
        }
        else{
            *error = true;
            break;
        }
        
        if (d + d_fill_count <= d_end){
            for (u32_4tech i = 0; i < d_fill_count; ++i){
                *d = d_fill[i];
                ++d;
            }
        }
        needed_max += d_fill_count;
    }
    
    return(needed_max);
}

#endif

// BOTTOM
