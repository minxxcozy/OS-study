#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PHYS_MEM_SIZE (64 * 1024)  // 64KB 물리 메모리 크기

typedef uint8_t  u8;
typedef uint32_t u32;


// 베이스 & 바운드 구조체
typedef struct {
    u32 base;    // 물리 메모리 시작 주소
    u32 bounds;  // 허용되는 가상 주소 범위 크기
} BaseBounds;


// 범위 초과 시 즉시 종료
void trap_oob(BaseBounds* bb, u32 vaddr, const char* op) {

    u32 max_addr = 0;

    if (bb->bounds > 0) {
        max_addr = bb->bounds - 1;
    } else {
        max_addr = 0;
    }

    printf("[범위 초과 오류]\n");
    printf("연산: %s\n", op);
    printf("요청한 가상주소: 0x%X\n", vaddr);
    printf("허용 범위: 0x0 ~ 0x%X\n", max_addr);

    exit(1);  
}


// 가상 주소 -> 물리 주소 변환
u32 translate_or_trap(BaseBounds* bb, u32 vaddr, const char* op) {

    // 1bounds 검사
    if (vaddr >= bb->bounds) {
        trap_oob(bb, vaddr, op);
    }

    // 물리 주소 계산
    u32 paddr = bb->base + vaddr;

    // 물리 메모리 범위 검사
    if (paddr >= PHYS_MEM_SIZE) {
        printf("[물리 주소 오류]\n");
        printf("계산된 물리주소: 0x%X\n", paddr);
        exit(1);
    }

    return paddr;
}


// 메모리 읽기
u8 mem_read(u8* mem, BaseBounds* bb, u32 vaddr) {

    u32 paddr = translate_or_trap(bb, vaddr, "읽기");

    return mem[paddr];
}


// 메모리 쓰기
void mem_write(u8* mem, BaseBounds* bb, u32 vaddr, u8 value) {

    u32 paddr = translate_or_trap(bb, vaddr, "쓰기");

    mem[paddr] = value;
}


int main() {
    // 64KB 물리 메모리
    static u8 phys[PHYS_MEM_SIZE];

    // 물리 메모리 0x2000 ~ 0x2FFF (4KB) 할당
    BaseBounds bb;
    bb.base = 0x2000;
    bb.bounds = 0x1000;

    // 정상 접근
    mem_write(phys, &bb, 0x10, 0xAB);

    u8 value = mem_read(phys, &bb, 0x10);

    printf("[정상 접근]\n");
    printf("가상주소 0x10 → 물리주소 0x%X\n", bb.base + 0x10);
    printf("읽은 값: 0x%X\n\n", value);

    // 범위 초과 접근 
    mem_read(phys, &bb, bb.bounds);

    return 0;
}