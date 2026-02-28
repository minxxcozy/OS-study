#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <iomanip>
using namespace std;

const int nloops = 1000000;     // 반복 횟수 (한 run 당 syscall 반복 실행 횟수)
const int runs   = 5;           // 평균을 내기 위한 반복 실험 횟수


// context를 받아서 1회 수행
typedef void (*op_fn)(void* ctx);       


struct BenchConfig {
    const char* timer_name;   // 타이머: gettimeofday / clock_gettime
    const char* op_name;      // syscall: getpid() / read(fd,NULL,0)
    bool use_clock_gettime;   // false: gettimeofday, true: clock_gettime
    
    int loops;                // 반복 횟수
    int runs;                 // 반복 실험 횟수
};


// 결과 저장
struct BenchResult {
    double total_avg_us;
};


// read() 실행 시 필요한 context (file descriptor)
struct ReadCtx {
    int fd;
};


// context 없이 getpid() 1회 실행
static void op_getpid(void* /*ctx*/) {
    (void)getpid();
}


// context로 전달된 fd를 이용해 read() 1회 실행
static void op_read(void* ctx) {
    ReadCtx* c = (ReadCtx*)ctx;
    (void)read(c->fd, nullptr, 0);
}


// gettimeofday
static long diff_us_tv(const timeval& s, const timeval& e) {
    return (e.tv_sec - s.tv_sec) * 1000000L + (e.tv_usec - s.tv_usec);
}


// clock_gettime
static long diff_ns_ts(const timespec& s, const timespec& e) {
    return (e.tv_sec - s.tv_sec) * 1000000000L + (e.tv_nsec - s.tv_nsec);
}


/*
수행 함수
- cfg: 벤치마크 설정
- op: 수행할 syscall 함수 
- ctx: syscall 함수에 전달할 context 
*/
static BenchResult run_bench(const BenchConfig& cfg, op_fn op, void* ctx) {
    BenchResult res{};
    res.total_avg_us = 0.0;

    cout << "=== " << cfg.timer_name << " | " << cfg.op_name << " ===\n";
    cout << fixed << setprecision(6);

    for (int r = 0; r < cfg.runs; r++) {
        double avg_us = 0.0;

        // gettimeofday
        if (!cfg.use_clock_gettime) {
            timeval s{}, e{};
            gettimeofday(&s, nullptr);

            for (int i = 0; i < cfg.loops; i++) op(ctx);

            gettimeofday(&e, nullptr);

            long diff_us = diff_us_tv(s, e);
            avg_us = (double)diff_us / cfg.loops;
        }
        
        // clock_gettime
        else {
            timespec s{}, e{};
            clock_gettime(CLOCK_MONOTONIC, &s);

            for (int i = 0; i < cfg.loops; i++) op(ctx);

            clock_gettime(CLOCK_MONOTONIC, &e);

            long diff_ns = diff_ns_ts(s, e);
            avg_us = (double)diff_ns / cfg.loops / 1000.0;      // ns -> us
        }

        res.total_avg_us += avg_us;
        cout << "Run " << r + 1 << ": " << avg_us << " us\n";
    }

    cout << "AVG : " << (res.total_avg_us / cfg.runs) << " us\n\n";
    return res;
}

int main() {
    // read() 테스트용 파일 디스크립터
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) {
        perror("open(/dev/null)");
        return 1;
    }

    // read()용 context 구성
    ReadCtx read_ctx{fd};

    // 설정
    BenchConfig tv_getpid { "gettimeofday", "getpid()",           false, nloops, runs };
    BenchConfig tv_read   { "gettimeofday", "read(fd,NULL,0)",    false, nloops, runs };
    BenchConfig clk_getpid{ "clock_gettime","getpid()",           true,  nloops, runs };
    BenchConfig clk_read  { "clock_gettime","read(fd,NULL,0)",    true,  nloops, runs };

    run_bench(tv_getpid,  op_getpid, nullptr);
    run_bench(tv_read,    op_read,   &read_ctx);
    run_bench(clk_getpid, op_getpid, nullptr);
    run_bench(clk_read,   op_read,   &read_ctx);

    close(fd);
    return 0;
}