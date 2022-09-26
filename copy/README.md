## 操作系统课程设计实验报告
**实验名称：复制文件(目录)**

<!-- @import "[TOC]" {cmd="toc" depthFrom=1 depthTo=6 orderedList=false} -->

<!-- code_chunk_output -->

- [操作系统课程设计实验报告](#操作系统课程设计实验报告)
  - [一、实验目的](#一-实验目的)
  - [二、实验内容](#二-实验内容)
  - [三、实验环境](#三-实验环境)
  - [四、程序设计与实现](#四-程序设计与实现)
    - [基本思路](#基本思路)
    - [windows](#windows)
    - [linux](#linux)
  - [五、实验结果](#五-实验结果)
    - [window](#window)
    - [linux](#linux-1)
  - [六、其他](#六-其他)

<!-- /code_chunk_output -->

 ---

### 一、实验目的
- 熟悉不同系统下如何操作文件。
- 学习在编程中实现对文件的访问，读写。
- 学习不同系统下文件属性，权限的读取与设置。
- 学习linux的链接机制。

### 二、实验内容
完成一个目录复制命令mycp，包括目录下的文件和子目录。复制后，不仅权限一致，而且时间属性也一致。linux下要求支持软链接。

### 三、实验环境
- CPU：intel core i7
- 内存：8G ddr4
- windows系统：win 10
- linux系统：Ubuntu 18.04.3
- 虚拟机平台：VMware 14
- linux 内核：linux 5.3.11
- 开发工具：Visual Studio 2017 社区版

### 四、程序设计与实现
#### 基本思路
复制非目录文件的大体流程为，将源文件读入内存，转到目标目录，新建文件，从源文件中读取数据再写入新文件。对于目录文件，由于存在子目录，所以可以用递归的方法实现。对于linux下的软连接，先得到其自身的文件属性，再得到它指向的文件，最后新建链接。

#### windows
- 在主函数中，将得到的参数转换为宽字符，若传入的参数为相对路径，则统一转换为绝对路径，方便后续处理。最后调用cp()函数完成复制。
    ```C++
    size_t len1 = strlen(argv[1]) + 1;
	size_t converted1 = 0;
	WCHAR source[200];
	mbstowcs_s(&converted1, source, len1, argv[1], _TRUNCATE);

	size_t len2 = strlen(argv[2]) + 1;
	size_t converted2 = 0;
	WCHAR target[200];
	mbstowcs_s(&converted2, target, len2, argv[2], _TRUNCATE);

	WCHAR SourceDir[200], TargetDir[200];
	if (argv[1][1] != ':')
	{
		GetCurrentDirectory(200, SourceDir);
		wcscat(SourceDir, L"\\");
		wcscat(SourceDir, source);
	}
	if (argv[2][1] != ':')
	{
		GetCurrentDirectory(200, TargetDir);
		wcscat(TargetDir, L"\\");
		wcscat(TargetDir, target);
	}

	cp(SourceDir, TargetDir);
    ```

- cp()函数的实现：
    - 调用FindFileFirst得到源文件的相关属性。同时得到其时间属性和权限属性。
        ```C++
        WIN32_FIND_DATA fd,tfd;
        HANDLE fh = FindFirstFile(s, &fd);

        wcscpy(tfd.cFileName, fd.cFileName);
        tfd.ftCreationTime = fd.ftCreationTime;
        tfd.ftLastAccessTime = fd.ftLastAccessTime;
        tfd.ftLastWriteTime = fd.ftLastWriteTime;
        
        DWORD attr = GetFileAttributes(s);
        ```

    - 判断源文件是不是目录文件，若是：
        - 记录当前目录。
        - 打开（不存在则创建）目标目录。
        - 在新目录创建源目录的副本。
        - 进入源文件(目录)遍历所有目录项，对于每一个目录项，更新目标目录，并递归调用cp()函数将其拷贝。
        - 回到最初的目标目录，为新复制的目录设置时间，权限属性。
        - 关闭相关句柄。
        ```C++
        if (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
        {
            WCHAR CurDir[200];
            GetCurrentDirectory(200, CurDir);

            if (!SetCurrentDirectory(t))
            {
                CreateDirectory(t, NULL);
                SetCurrentDirectory(t);
            }
            
            CreateDirectory(fd.cFileName, NULL);

            SetCurrentDirectory(CurDir);
            SetCurrentDirectory(fd.cFileName);

            WCHAR source[200], target[200], nsource[200];
            wcscpy(source, s);
            wcscpy(target, t);
            wcscat(source, L"\\*.*");
            wcscat(target, L"\\");
            wcscat(target, fd.cFileName);

            HANDLE sfh = FindFirstFile(source, &fd);
            //cp(fd.cFileName, target);

            while (FindNextFile(sfh, &fd))
            {
                if (fd.cFileName[0] == '.')
                    continue;
                wcscpy(nsource, s);
                wcscat(nsource, L"\\");
                wcscat(nsource, fd.cFileName);
                cp(nsource, target);
            }
            
            SetCurrentDirectory(t);
            HANDLE dir = CreateFile(tfd.cFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
            //SetFileAttributes()
            SetFileTime(dir, &tfd.ftCreationTime, &tfd.ftLastAccessTime, &tfd.ftLastWriteTime);
            
            SetFileAttributes(tfd.cFileName, attr);
            CloseHandle(dir);

            FindClose(sfh);
        }
        ```
 
    - 若不是目录文件，则：
        - 调用CreateFile()打开源文件。
        - 打开（不存在则创建）目标目录。
        - 在新目录创建源文件的副本。
        - 使用ReadFile()从源文件读取数据，用WriteFile()将数据写入新文件。
        - 为新文件设置时间，权限属性。
        - 关闭相关句柄。
        ```C++
        else
        {
            HANDLE fs = CreateFile(
                fd.cFileName,
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );

            if (!SetCurrentDirectory(t))
            {
                CreateDirectory(t, NULL);
                SetCurrentDirectory(t);
            }

            HANDLE nfs = CreateFile(
                fd.cFileName,
                GENERIC_WRITE,
                FILE_SHARE_WRITE,
                NULL,
                CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );

            DWORD rlen, wlen;
            char *buffer;
            buffer = (char*)malloc((fd.nFileSizeHigh * (MAXDWORD + 1)) + fd.nFileSizeLow) + 1;
            ReadFile(fs, buffer, (fd.nFileSizeHigh * (MAXDWORD + 1)) + fd.nFileSizeLow, &rlen, NULL);
            WriteFile(nfs, buffer, rlen, &wlen, NULL);

            SetFileAttributes(fd.cFileName, attr);
            SetFileTime(nfs, &fd.ftCreationTime, &fd.ftLastAccessTime, &fd.ftLastWriteTime);
            
            CloseHandle(fs);
            CloseHandle(nfs);
        }
        ```

#### linux
- 在主程序内，若参数为相对路径，将其转换为绝对路径。调用cp()函数。
    ```C++
    char source[200], target[200];

	if (argv[1][0] != '/')
	{
		char *curdir = getcwd(NULL, 0);
		strcpy(source, curdir);
		strcat(source, "/");
		strcat(source, argv[1]);
	}
	
	realpath(argv[2], target);

	cp(source, target);
    ```
 
 - cp()函数的实现：
    - 使用lstat()得到源文件的信息，并记录其时间信息。将源文件的文件名从路径中分离出来(ns)，方便后续处理。
    ```C++
    struct stat date;
	lstat(s, &date);

	struct utimbuf mytime;
	mytime.actime = date.st_atim.tv_sec;
	mytime.modtime = date.st_mtim.tv_sec;

	char d[200], ns[200];
	int j = 0, i = 0;
	for (i = strlen(s); s[i] != '/'; i--)
		d[j++] = s[i];
	i = 0;
	while (j >= 0)
		ns[i++] = d[--j];
    ```
 
    - 判断源文件类型，若为目录文件，处理逻辑与windows下类似。区别在于用mkdir()新建目录，用chdir()转到目录，用chmod()，utime()设置权限和时间等等。
        ```C++
        if (S_ISDIR(date.st_mode))
        {
            char *mcurdir = getcwd(NULL, 0);
            if (chdir(t))
            {
                mkdir(t, 0777);
                chdir(t);
            }

            mkdir(ns, date.st_mode);

            char target[200];
            strcpy(target, t);
            strcat(target, "/");
            strcat(target, ns);

            chdir(mcurdir);
            chdir(s);

            DIR *dir = opendir(".");
            struct dirent *ddate;
            while (ddate = readdir(dir))
            {
                if (ddate->d_name[0] == '.')
                    continue;
                chdir(s);
                char ss[200];
                
                char source[200];
                char *cur = getcwd(NULL, 0);
                strcpy(source, cur);
                strcat(source, "/");
                strcat(source, ddate->d_name);

                cp(source, target);
            }

            chdir(t);
            chmod(ns, date.st_mode);
            utime(ns, &mytime);

            closedir(dir);
        }
        ```
	
    - 若为链接文件，则：
        - 将链接文件名拼接在目标路径后作为新链接文件的路径target。
        - 使用realpath()得到链接文件指向的的真实路径lk。
        - 使用symlink(lk，target)；创建新的链接文件。
        - 复制原链接文件的时间信息。
        - 使用lchmod()和lutimes()设置新链接文件的权限。
        ```C++
        else if (S_ISLNK(date.st_mode))
        {
            char lk[200];
            char target[200];
            strcpy(target, t);
            strcat(target, "/");
            strcat(target, ns);
            
            realpath(s, lk);
            symlink(lk, target);
            
            timeval ltime[2];
            ltime[0].tv_sec = date.st_atim.tv_sec;
            ltime[0].tv_usec = date.st_atim.tv_nsec / 1000;
            ltime[1].tv_sec = date.st_mtim.tv_sec;
            ltime[1].tv_usec = date.st_mtim.tv_nsec / 1000;
            
            chdir(t);
            lchmod(ns, date.st_mode);
            lutimes(ns, ltime);
        }
        ```

    - 若为或非目录非链接文件，处理逻辑与windows下类似。区别在于用mkdir()新建目录，用chdir()转到目录，用chmod()，utime()设置权限和时间等等。
        ```C++
        else
        {
            int file = open(s, O_RDONLY);

            if (chdir(t))
            {
                mkdir(t, 0777);
                chdir(t);
            }

            int nfile = creat(ns, date.st_mode);
            char *buffer;
            buffer = (char*)malloc(date.st_size + 1);
            read(file, buffer, date.st_size);
            write(nfile, buffer, date.st_size);

            chmod(ns, date.st_mode);
            utime(ns, &mytime);

            close(file);
            close(nfile);
        }
        ```

### 五、实验结果
#### window
![win][pic-0]
![win][pic-1]

#### linux
![linux][pic-2]

### 六、其他
- 修改目录时间属性语句的位置。由于程序是递归实现，不能在目录创建时直接修改，因为后续的操作会在该目录下添加文件，新的操作会覆盖前面的修改，所以必须在程序递归处理完所有目录项后才可以修改其时间属性，除外，修改链接文件的时间属性时也遇到了麻烦，由于utime()会默认修改链接文件所指向的文件的时间属性，所以不能用，查了好久才找到lutimes()这个函数。相同的还有lchmod()。
- linux下处理链接文件，由于用readlink()得到的源文件路径是相对路径，复制后新的链接文件可能无法定位到真实的源文件，最终想到一个办法，直接用realpath()得到链接文件所指向的文件的绝对路径，建立新的链接，就可以使复制后的的链接文件指向绝对路径。类似的问题还有，由于链接文件，在向cp()函数传入源文件名前，不能直接用realpath()去取绝对路径，必须手动将其变为绝对路径。

---

[pic-0]: https://images.wait4echo.love/Works/os-experiment/ose-cp-0.png
[pic-1]: https://images.wait4echo.love/Works/os-experiment/ose-cp-1.png
[pic-2]: https://images.wait4echo.love/Works/os-experiment/ose-cp-2.png



		


