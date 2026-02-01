# Overview: Process API 실습 코드 (Problem 1-8)
각 프로그램은 `fork()`, `wait()`, `exec()`, `open()`, `pipe()` 등 프로세스 관련 시스템 콜을 사용하여,
프로세스 생성, 종료, 동기화, 주소 공간 분리, 파일 디스크립터 공유, 그리고 프로세스 간 통신
어떻게 동작하는지를 직접 관찰하는 것을 목표로 한다.

## Problem 1: fork 이후 주소 공간 (Address Space)
> [p1_fork_address_space.c](https://github.com/minxxcozy/OS-study/blob/main/05-Process-API/code/p1_fork_address_space.c)

`fork()` 호출 이후, 부모 프로세스와 자식 프로세스가 **각각 독립적인 주소 공간**을 가진다는 사실을 보여준다.

부모 프로세스에서 변수 `x`에 값을 할당한 뒤 `fork()`를 호출하면,
자식 프로세스는 해당 시점의 값을 복사하여 시작한다.
이후 부모와 자식이 각각 `x`의 값을 변경하더라도,
서로의 값에는 영향을 주지 않는다.

`fork()`가 단순히 실행 흐름을 분기하는 것이 아니라, 부모의 주소 공간을 기반으로
**새로운 프로세스의 논리적 메모리 공간을 생성**함을 의미한다.
실제 운영체제에서는 성능을 위해 Copy-on-Write 기법이 사용되지만,
프로그래머 관점에서는 부모와 자식이 완전히 분리된 메모리를 가진 것처럼 동작한다.

❗ `fork()`는 스레드 생성이 아니라, **주소 공간을 분리한 프로세스 복제**임을 확인할 수 있다.



## Problem 2: open 후 fork와 파일 디스크립터 공유
> [p2_fork_file_descriptor.c](https://github.com/minxxcozy/OS-study/blob/main/05-Process-API/code/p2_fork_file_descriptor.c)

`open()`으로 파일을 연 뒤 `fork()`를 호출했을 때,
부모와 자식 프로세스가 **같은 파일 디스크립터를 통해 파일에 접근할 수 있음**을 보여준다.

`fork()` 이후 각 프로세스는 독립적인 주소 공간을 가지지만,
파일 디스크립터는 예외적으로 공유될 수 있다.
부모와 자식은 동일한 파일 디스크립터를 사용하여 파일에 데이터를 기록할 수 있으며,
실제 파일에 기록되는 순서는 프로세스가 스케줄링되는 순서에 따라 달라진다.

이는 파일 디스크립터가 단순한 정수가 아니라,
커널 내부의 **열린 파일 상태**를 참조하는 추상화임을 보여준다.
따라서 파일 오프셋 또한 공유되며, 동시 접근 환경에서는 결과가 비결정적으로 나타날 수 있다.

❗ 주소 공간은 분리되지만, 파일 디스크립터와 같은 커널 자원은 의도적으로 공유될 수 있다.


## Problem 3: wait을 이용한 실행 순서 제어
> [p3_fork_wait_synchronization.c](https://github.com/minxxcozy/OS-study/blob/main/05-Process-API/code/p3_fork_wait_synchronization.c)

자식 프로세스가 먼저 "hello?"를 출력하고,
부모 프로세스가 그 이후에 "goodbye!"를 출력하도록 구성되어 있다.

부모 프로세스에서 `wait()`를 호출함으로써,
자식 프로세스가 종료될 때까지 실행을 멈추고 대기 상태에 들어간다.
이로 인해 부모의 출력은 자식 프로세스가 종료된 이후에만 수행된다.

해당 과제는 출력 순서를 제어하기 위해 단순한 시간 지연이 아니라,
운영체제가 제공하는 **명시적인 동기화 메커니즘**이 필요하다는 점을 보여준다.
`wait()`는 부모 프로세스를 block 상태로 만들고,
운영체제는 해당 프로세스를 일시적으로 스케줄링 대상에서 제외한다.
즉, `wait()`는 실행 흐름 제어와 자원 회수를 통시에 수행하는 시스템 콜이다.

❗ 실행 순서를 보장하려면 `sleep()`이 아니라 동기화 시스템 콜을 사용해야 한다.



## Problem 4: fork 이후 exec을 이용한 프로그램 교체
> [p4_fork_exec_replacement.c](https://github.com/minxxcozy/OS-study/blob/main/05-Process-API/code/p4_fork_exec_replacement.c)

`fork()`로 자식 프로세스를 생성한 뒤,
자식 프로세스에서 `exec()` 계열 함수를 사용하여 `/bin/ls` 프로그램을 실행한다.

`exec()`는 새로운 프로그램을 생성하는 것이 아니라,
현재 실행 중인 프로세스의 코드, 스택, 힙을 완전히 다른 프로그램으로 **교체**한다.
이 과정에서 프로세스의 PID는 유지되지만, 실행 중이던 프로그램의 내용은 사라진다.

이를 통해 Unix 계열 운영체제는 프로세스 생성(fork)과 프로그램 실행(exec)을 명확히 분리하여 설계된 것을 알 수 있다.
이러한 구조는 셸이 입출력 재지정이나 파이프 설정을 `exec()` 이전에 유연하게 구성할 수 있게 해준다.

❗ `fork()`는 프로세스를 만들고, `exec()`는 그 프로세스가 무엇을 실행할지를 결정한다.



## Problem 5: wait의 반환값과 동작 방식
> [p5_wait_semantics.c](https://github.com/minxxcozy/OS-study/blob/main/05-Process-API/code/p5_wait_semantics.c)

부모 프로세스가 `wait()`를 호출했을 때 어떤 값이 반환되는지를 확인한다.

`wait()`는 종료한 자식 프로세스의 PID를 반환하며,
자식 프로세스가 아직 종료되지 않았다면 부모 프로세스는 대기 상태에 들어간다.
자식 프로세스는 `exit()` 호출 이후 즉시 완전히 제거되지 않고,
부모가 `wait()`로 상태를 수거하기 전까지 일시적으로 **Zombie 상태**로 남는다. (시스템에 Zombie 프로세스 누적)

반면 자식 프로세스가 `wait()`를 호출할 경우, 기다릴 자식이 존재하지 않으므로 정상적으로 동작하지 않는다.

❗ `wait()`는 단순한 대기 함수가 아니라, 자식 프로세스의 종료를 정리하는 역할을 수행한다.



## Problem 6: waitpid를 이용한 제어된 대기
> [p6_waitpid_control.c](https://github.com/minxxcozy/OS-study/blob/main/05-Process-API/code/p6_waitpid_control.c)

`wait()` 대신 `waitpid()`를 사용하여 특정 자식 프로세스가 종료되기를 기다린다.

여러 개의 자식 프로세스를 생성하는 환경에서는 종료 순서를 예측할 수 없기 때문에
`waitpid()`를 사용하면 어떤 자식을 기다릴지 명시적으로 제어할 필요가 있다.
이는 서버나 병렬 프로그램과 같이 다수의 자식 프로세스를 관리하는 상황에서 특히 중요한 메커니즘이다.


## Problem 7: 표준 출력과 파일 디스크립터
> [p7_fd_close_stdout.c](https://github.com/minxxcozy/OS-study/blob/main/05-Process-API/code/p7_fd_close_stdout.c)

자식 프로세스에서 표준 출력(`STDOUT_FILENO`)을 닫은 뒤 `printf()`를 호출했을 때의 동작을 관찰한다.

표준 출력이 닫힌 상태에서는 출력이 화면에 나타나지 않으며,
이를 통해 표준 출력 또한 파일 디스크립터로 **추상화**된 자원임을 확인할 수 있다.
```
Process
 └── File Descriptor Table
       fd 0 → stdin  → 어떤 열린 파일/장치
       fd 1 → stdout → 어떤 열린 파일/장치
       fd 2 → stderr → 어떤 열린 파일/장치
       fd 3 → ...
```
* `STDOUT_FILENO`는 화면이 아니라, **파일 디스크립터 테이블의 인덱스** (커널 내부의 열린 파일 객체를 가리키는 참조)
* 파일 디스크립터 번호는 커널이 파일 열기, 리다이렉션, 파이프 설정 시점에 규칙적으로 할당된다.

이러한 추상화 덕분에 파일, 파이프, 소켓과 같은 다양한 입출력 대상이 동일한 인터페이스로 다뤄질 수 있다.

❗ Unix 계열 운영체제에서 입출력은 파일 디스크립터를 중심으로 설계되어 있다.


## Problem 8: pipe를 이용한 프로세스 간 통신
> [p8_pipe_ipc.c](https://github.com/minxxcozy/OS-study/blob/main/05-Process-API/code/p8_pipe_ipc.c)

`pipe()` 시스템 콜을 사용하여 프로세스 간 통신(IPC)을 구성한다.

하나의 프로세스는 파이프에 데이터를 쓰고, 다른 프로세스는 해당 데이터를 읽는다.
`fork()` 이후에도 파일 디스크립터가 공유되기 때문에, 부모-자식 또는 형제 프로세스 간 통신이 가능하다.

**파이프**를 통한 읽기와 쓰기 과정에서는 프로세스가 봉쇄되거나 깨어나며,
이 과정에서 문맥 교환이 발생할 수 있다. 
