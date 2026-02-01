# OS-study
운영체제 학습을 위해 **OSTEP (Operating Systems: Three Easy Pieces)** 를 기반으로,  
개념 정리 + 실습 코드 (과제)를 함께 정리한 레포지토리

## Reference
* [OSTEP (Korean Translation)](https://github.com/remzi-arpacidusseau/ostep-translations/tree/master/korean)
* [OSTEP Homework & Experiments](https://github.com/remzi-arpacidusseau/ostep-homework)

## Study Plan
* **분할 1 ~ 5 :** Intro + Virtualization
* **분할 6 ~ 7 :** Concurrency
* **분할 8 ~ 10 :** Persistence & Distributed Systems

| 분할     | 챕터 범위     | 포함 내용 요약                                                     |
| ------ | --------- | ------------------------------------------------------------ |
| 1      | 1–6       | OS 기초, 실행 모델, 스케줄링 대화형 섹션                                    |
| 2      | 7–11      | CPU/멀티코어/스케줄링 알고리즘 + Summary                                 |
| 3      | 12–16     | 가상화, 주소공간, 메모리 변환/세그먼테이션                                     |
| 4      | 17–19     | Paging 기초, TLB, 페이지 테이블 심화                                   |
| **5**  | **20–24** | **Swapping (VAX 사례 포함) + 메모리 정책**                            |
| 6      | 25–28     | Concurrency 개념, 스레드, Thread API, Lock                        |
| **7**  | **29–34** | **조건변수, 세마포어, 이벤트 기반 동시성 + Summary**                         |
| 8      | 35–40     | Persistence (I/O, 디스크, 파일/디렉토리, FS 구현)                       |
| 9      | 41–45     | FFS, Journaling/LFS, Data Integrity + Summary                |
| **10** | **46–50** | **Distributed Systems, NFS/AFS, Network FS + Final Summary** |

## Repository Structure
각 디렉토리는 **전공책 챕터 번호 + 주제**를 기준으로 구성
```
01-introduction
02-os-structures
03-process
04-process-api
05-cpu-scheduling
...
```

각 터 디렉토리 내부 구조
```
00-topic/
 ├── README.md        # 핵심 개념 요약
 └── code/            # 실습 코드 (HW)
