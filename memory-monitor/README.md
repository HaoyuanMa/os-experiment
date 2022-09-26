## 操作系统课程设计实验报告
**实验名称：内存监视**


<!-- @import "[TOC]" {cmd="toc" depthFrom=1 depthTo=6 orderedList=false} -->

<!-- code_chunk_output -->

- [操作系统课程设计实验报告](#操作系统课程设计实验报告)
  - [一、实验目的](#一-实验目的)
  - [二、实验内容](#二-实验内容)
  - [三、实验环境](#三-实验环境)
  - [四、程序设计与实现](#四-程序设计与实现)
  - [五、实验结果](#五-实验结果)

<!-- /code_chunk_output -->

 ---

### 一、实验目的
- 学习编程查询系统内存布局及内存使用情况。
- 了解windows系统的内存机制。

### 二、实验内容
设计一个内存监视器，能实时地显示当前系统中内存的使用情况，包括系统地址空间的布局，物理内存的使用情况；能实时显示某个进程的虚拟地址空间布局和工作集信息等。

### 三、实验环境
- CPU：intel core i7
- 内存：8G ddr4
- windows系统：win 10
- 开发工具：Visual Studio 2017 社区版

### 四、程序设计与实现
- Print Menu
    - 负责打印用户交互界面。
        ```C++
        void PrintMenu()
        {
            printf("%s* * * * * * * * * * * * * * * * * * * * * * * * *\n", BK);
            printf("%s*                  Memory Monitor               *\n", BK);
            printf("%s*                                               *\n", BK);
            printf("%s*  a.Show Performance Information               *\n", BK);
            printf("%s*  b.Show Memory Status                         *\n", BK);
            printf("%s*  c.Show System Information                    *\n", BK);		
            printf("%s*  d.Query All Process Control Information      *\n", BK);
            printf("%s*  e.Query Single Process Control Information   *\n", BK);
            printf("%s*                                               *\n", BK);
            printf("%s* * * * * * * * * * * * * * * * * * * * * * * * *\n", BK);
            printf("\n\n\nPlease choose function.\n( q to quit. )\n");
        }
        ```

- Show Performance Information
    - 使用系统调用GetPerformanceInfo()获得存储器信息并打印。
        ```C++
        void a()
        {
            struct _PERFORMANCE_INFORMATION
                pi;
            GetPerformanceInfo(&pi, sizeof(pi));
            printf("\n\n");
            printf("%sCommit Total:                  | %u\n", BK, pi.CommitTotal);
            printf("%sCommit Limit:                  | %u\n", BK, pi.CommitLimit);
            printf("%sCommit Peak:                   | %u\n", BK, pi.CommitPeak);
            printf("%sPhysical Total:                | %u\n", BK, pi.PhysicalTotal);
            printf("%sPhysical Available:            | %u\n", BK, pi.PhysicalAvailable);
            printf("%sSystem Cache:                  | %u\n", BK, pi.SystemCache);
            printf("%sKernel Total:                  | %u\n", BK, pi.KernelTotal);
            printf("%sKernel Paged:                  | %u\n", BK, pi.KernelPaged);
            printf("%sKernel Nonpaged:               | %u\n", BK, pi.KernelNonpaged);
            printf("%sPage Size                      | %.2f KB\n", BK, pi.PageSize / 1024.0);
            printf("%sHandle Count:                  | %u\n", BK, pi.HandleCount);
            printf("%sProcess Count:                 | %u\n", BK, pi.ProcessCount);
            printf("%sThread Count:                  | %u\n", BK, pi.ThreadCount);
            printf("\n\n");
        }
        ```

- Show Memory Status
    - 使用系统调用GlobalMemoryStatusEx ()获得存储器信息并打印。
        ```C++
        void b()
        {
            struct _MEMORYSTATUSEX mi;
            mi.dwLength = sizeof(mi);
            GlobalMemoryStatusEx(&mi);
            printf("\n\n");
            printf("%sMemory Load:                    | %.2f%%\n", BK, (float)mi.dwMemoryLoad);
            printf("%sTotle Memory:                   | %.2f GB\n", BK, mi.ullTotalPhys / 1024.0 / 1024.0 / 1024.0);
            printf("%sAvail Memory:                   | %.2f GB\n", BK, mi.ullAvailPhys / 1024.0 / 1024.0 / 1024.0);
            printf("%sTotal PageFile:                 | %.2f GB\n", BK, mi.ullTotalPageFile / 1024.0 / 1024.0 / 1024.0);
            printf("%sAvail PageFile                  | %.2f GB\n", BK, mi.ullAvailPageFile / 1024.0 / 1024.0 / 1024.0);
            printf("%sTotal Virtual:                  | %.2f GB\n", BK, mi.ullTotalVirtual / 1024.0 / 1024.0 / 1024.0);
            printf("%sAvail Virtual:                  | %.2f GB\n", BK, mi.ullAvailVirtual / 1024.0 / 1024.0 / 1024.0);
            printf("\n\n");
        }    
        ```
- Show System Information
    - 使用系统调用GetSystemInfo ()获得存储器信息并打印。
        ```C++
        void c()
        {
            SYSTEM_INFO si;
            GetSystemInfo(&si);
            printf("\n\n");
            printf("%sPage Size:                      | %d KB\n", BK, (int)si.dwPageSize / 1024);
            printf("%sMinimumApplicationAddress:      | 0x%0.8x \n", BK, si.lpMinimumApplicationAddress);
            printf("%sMaximumApplicationAddress:      | 0x%0.8x \n", BK, si.lpMaximumApplicationAddress);
            printf("%sNumberOfProcessors:             | %d\n", BK, si.dwNumberOfProcessors);
            printf("%sProcessor Type                  | %d\n", BK, si.dwProcessorType);
            printf("%sAllocation Granularity:         | %d KB\n", BK, (int)si.dwAllocationGranularity / 1024);
            printf("%sProcessor Level:                | %d\n", BK, si.wProcessorLevel);
            printf("\n\n");
        }
        ```

- Query All Process Control Information
    - 得到当前进程信息的快照，并查询第一个，将进程信息存入pe。
        ```C++
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(pe);
        HANDLE hps = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        int find = Process32First(hps, &pe);
        ```

    - 循环查询快照中所有进程信息并输出，hp存放打开进程的句柄，pmc存放进程内存信息。
        ```C++
        while (find)
            {
                if (pid == -1 || pe.th32ProcessID == pid)
                {
                    //SetLastError(0);
                    
                    HANDLE hp = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
                    
                    //printf("error: %d\n", GetLastError());

                    PROCESS_MEMORY_COUNTERS pmc;
                    GetProcessMemoryInfo(hp, &pmc, sizeof(pmc));
                
                    if (pid == -1)
                    {
                        wprintf(L"%12d                        %ls", pe.th32ProcessID, pe.szExeFile);
                        for (int i = 1; i <= (70 - (int)wcslen(pe.szExeFile)); i++)
                            printf(" ");
                        printf("%.2fKB\n", pmc.WorkingSetSize / 1024.0);
                    }
                    else
                    {
                        printf("%sProcess Id:                     | %d\n", BK, pe.th32ProcessID);
                        wprintf(L"                        Process Name:                   | %ls\n", pe.szExeFile);
                        printf("%sWorkingSet Size:                | %.2fKB\n", BK, pmc.WorkingSetSize / 1024.0);
                    }

                    MEMORY_BASIC_INFORMATION mbi;
                    LPCVOID pb = (LPVOID)si.lpMinimumApplicationAddress;

                    if(pid != -1)
                        printf("%sMemory Information:\n", BK);

                    //......
                }
            }
        ```

- Query Single Process Control Information
    - 输出进程id，名称等基本信息。从程序可寻址的最低地址开始查询进程各内存块的大小状态等。
        ```C++
        while (pid!=-1 && pb < si.lpMaximumApplicationAddress)
        {
            
            VirtualQueryEx(hp, pb, &mbi, sizeof(mbi));
            LPCVOID ped = (PBYTE)pb + mbi.RegionSize;
            
            printf("%s0x%8x - 0x%8x (%.2f MB)    state: ", BK, pb, ped, mbi.RegionSize / 1024.0 / 1024.0);
            
            switch (mbi.State)
            {
            case MEM_COMMIT:
                printf("COMMIT  ");
                break;
            case MEM_FREE:
                printf("FREE    ");
                break;
            case MEM_RESERVE:
                printf("RESERVE ");
                break;
            }
            printf("   type: ");
            switch (mbi.Type)
            {
            case MEM_IMAGE:
                printf("IMAGE ");
                break;
            case MEM_MAPPED:
                printf("MAPPED ");
                break;
            case MEM_PRIVATE:
                printf("PRIVATE ");
                break;
            }
            printf("\n");
            pb = ped;	
        }
        ```
### 五、实验结果
- Show Performance Information
![A][pic-A]
- Show Memory Status
![B][pic-B]
- Show System Information
![C][pic-C]
- Query All Process Control Information
![DA][pic-DA]
- Query Single Process Control Information
![DS][pic-DS]
---

[pic-A]: https://images.wait4echo.love/Works/os-experiment/ose-mm-0.png
[pic-B]: https://images.wait4echo.love/Works/os-experiment/ose-mm-1.png
[pic-C]: https://images.wait4echo.love/Works/os-experiment/ose-mm-2.png
[pic-DA]: https://images.wait4echo.love/Works/os-experiment/ose-mm-3.png
[pic-DS]: https://images.wait4echo.love/Works/os-experiment/ose-mm-4.png



		


