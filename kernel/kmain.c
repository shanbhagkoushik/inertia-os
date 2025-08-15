#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint32_t total_size;
    uint32_t reserved;
} mboot2_info_t;

typedef struct {
    uint32_t type;
    uint32_t size;
} mboot2_tag_t;


typedef struct {
    uint32_t type;
    uint32_t size;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint16_t reserved;
    
} mboot2_tag_fb_t;

static inline void *mb2_find_tag(uint64_t mb_addr, uint32_t want_type) {
    mboot2_info_t *info = (mboot2_info_t*)(uintptr_t)mb_addr;
    uint8_t *p = (uint8_t*)info + 8; 
    uint8_t *end = (uint8_t*)info + info->total_size;

    while (p < end) {
        mboot2_tag_t *tag = (mboot2_tag_t*)p;
        if (tag->type == 0) break; 
        if (tag->type == want_type) return tag;
       
        p += ((tag->size + 7) & ~7);
    }
    return NULL;
}


typedef struct {
    uint32_t *ptr;
    uint32_t pitch;  
    uint32_t w, h;
    uint8_t  bpp;
} fb_t;

static fb_t FB = {0};

static inline void set_pixel(int x, int y, uint32_t rgba) {
    if (!FB.ptr) return;
    if ((unsigned)x >= FB.w || (unsigned)y >= FB.h) return;
    uint32_t *row = (uint32_t*)( (uint8_t*)FB.ptr + (size_t)y * FB.pitch );
    row[x] = rgba;
}

static void fill_rect(int x, int y, int w, int h, uint32_t rgba) {
    if (!FB.ptr) return;
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > (int)FB.w) w = FB.w - x;
    if (y + h > (int)FB.h) h = FB.h - y;
    if (w <= 0 || h <= 0) return;

    for (int yy = y; yy < y + h; ++yy) {
        uint32_t *row = (uint32_t*)((uint8_t*)FB.ptr + (size_t)yy * FB.pitch);
        for (int xx = x; xx < x + w; ++xx) row[xx] = rgba;
    }
}

static void fill_round_rect(int x,int y,int w,int h,int r,uint32_t rgba){
    if (r*2 > w) r = w/2;
    if (r*2 > h) r = h/2;
    
    fill_rect(x+r, y, w-2*r, h, rgba);
    fill_rect(x, y+r, r, h-2*r, rgba);
    fill_rect(x+w-r, y+r, r, h-2*r, rgba);
    
    for(int yy=-r; yy<=r; ++yy){
        int dx = (int)(r - (int)((yy<0?-yy:yy))); 
        for(int xx=-dx; xx<=dx; ++xx){
            set_pixel(x+r+xx, y+r+yy, rgba);
            set_pixel(x+w-r-1+xx, y+r+yy, rgba);
            set_pixel(x+r+xx, y+h-r-1+yy, rgba);
            set_pixel(x+w-r-1+xx, y+h-r-1+yy, rgba);
        }
    }
}

static void draw_menu_bar(){
   
    uint32_t bar = 0xFFEFEFEF; 
    fill_rect(0, 0, FB.w, 28, bar);

    
    int cx = 14, cy = 14, r = 6;
    uint32_t red = 0xFFFF5F57, yellow = 0xFFFFBD2E, green = 0xFF28C840;
    for(int yy=-r; yy<=r; ++yy){
        for(int xx=-r; xx<=r; ++xx){
            if (xx*xx + yy*yy <= r*r) {
                set_pixel(cx+xx, cy+yy, red);
                set_pixel(cx+xx+22, cy+yy, yellow);
                set_pixel(cx+xx+44, cy+yy, green);
            }
        }
    }
}

static void draw_dock(){
    int dock_w = FB.w * 3 / 5;
    int dock_h = 64;
    int dock_x = (FB.w - dock_w)/2;
    int dock_y = FB.h - dock_h - 24;
    uint32_t dock_col = 0xF0F4F4F4; 
    fill_round_rect(dock_x, dock_y, dock_w, dock_h, 18, dock_col);

   
    int cx = dock_x + 40;
    int cy = dock_y + dock_h/2;
    int r = 20;
    uint32_t icon1 = 0xFF6AA9FF, icon2 = 0xFFFF8A5B, icon3 = 0xFF9BDB7A, icon4 = 0xFFB18AE0;

    uint32_t colors[4] = {icon1, icon2, icon3, icon4};
    for (int i=0;i<4;i++){
        int icx = cx + i*80;
        for(int yy=-r; yy<=r; ++yy)
            for(int xx=-r; xx<=r; ++xx)
                if (xx*xx + yy*yy <= r*r) set_pixel(icx+xx, cy+yy, colors[i]);
    }
}

static void clear_desktop(){
   
    uint32_t bg = 0xFFF7F7F9; 
    fill_rect(0, 0, FB.w, FB.h, bg);
}

void kmain(uint64_t mboot_addr) {
    
    mboot2_tag_fb_t *fbtag = (mboot2_tag_fb_t*)mb2_find_tag(mboot_addr, 8);
    if (!fbtag || fbtag->framebuffer_bpp != 32) {
        
        for(;;) { __asm__ __volatile__("hlt"); }
    }

    FB.ptr  = (uint32_t*)(uintptr_t)fbtag->framebuffer_addr;
    FB.pitch= fbtag->framebuffer_pitch;
    FB.w    = fbtag->framebuffer_width;
    FB.h    = fbtag->framebuffer_height;
    FB.bpp  = fbtag->framebuffer_bpp;

    clear_desktop();
    draw_menu_bar();
    draw_dock();

    for(;;) { __asm__ __volatile__("hlt"); }
}
