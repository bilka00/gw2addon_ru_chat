# gw2addon_ru_chat

Thank Megai2

based on https://github.com/gw2-addon-loader/GW2Radial

Reverse engineering gw2 
```
00007FF69A7835A0
00007FF69A7835A0 | 48:895C24 08             | mov qword ptr ss:[rsp+8],rbx                               |
00007FF69A7835A5 | 57                       | push rdi                                                   |
00007FF69A7835A6 | 48:83EC 20               | sub rsp,20                                                 |
00007FF69A7835AA | 48:63DA                  | movsxd rbx,edx                                             | rbx:"C&\n"
00007FF69A7835AD | 48:8BF9                  | mov rdi,rcx                                                |
00007FF69A7835B0 | 83FB 28                  | cmp ebx,28                                                 | 28:'('
00007FF69A7835B3 | 75 1B                    | jne gw2-64.7FF69A7835D0                                    |
00007FF69A7835B5 | 41:B8 FA070000           | mov r8d,7FA                                                |
00007FF69A7835BB | 48:8D15 8E922601         | lea rdx,qword ptr ds:[7FF69B9EC850]                        | 00007FF69B9EC850:"..\\..\\..\\Game\\Chat\\Cli\\CtCliContext.cpp"
00007FF69A7835C2 | 48:8D0D 77942601         | lea rcx,qword ptr ds:[7FF69B9ECA40]                        | 00007FF69B9ECA40:"channel != CHAT_CHANNEL_CUSTOM"
00007FF69A7835C9 | E8 F297E0FF              | call gw2-64.7FF69A58CDC0                                   |
00007FF69A7835CE | EB 25                    | jmp gw2-64.7FF69A7835F5                                    |
00007FF69A7835D0 | 83FB 29                  | cmp ebx,29                                                 | 29:')'
00007FF69A7835D3 | 75 1B                    | jne gw2-64.7FF69A7835F0                                    |
00007FF69A7835D5 | 41:B8 FB070000           | mov r8d,7FB                                                |
00007FF69A7835DB | 48:8D15 6E922601         | lea rdx,qword ptr ds:[7FF69B9EC850]                        | 00007FF69B9EC850:"..\\..\\..\\Game\\Chat\\Cli\\CtCliContext.cpp"
00007FF69A7835E2 | 48:8D0D 77942601         | lea rcx,qword ptr ds:[7FF69B9ECA60]                        | 00007FF69B9ECA60:"channel != CHAT_CHANNEL_WHISPER"
00007FF69A7835E9 | E8 D297E0FF              | call gw2-64.7FF69A58CDC0                                   |
00007FF69A7835EE | EB 05                    | jmp gw2-64.7FF69A7835F5                                    |
00007FF69A7835F0 | 83FB 28                  | cmp ebx,28                                                 | 28:'('
00007FF69A7835F3 | 72 19                    | jb gw2-64.7FF69A78360E                                     |
00007FF69A7835F5 | 41:B8 FC070000           | mov r8d,7FC                                                |
00007FF69A7835FB | 48:8D15 4E922601         | lea rdx,qword ptr ds:[7FF69B9EC850]                        | 00007FF69B9EC850:"..\\..\\..\\Game\\Chat\\Cli\\CtCliContext.cpp"
00007FF69A783602 | 48:8D0D 77942601         | lea rcx,qword ptr ds:[7FF69B9ECA80]                        | 00007FF69B9ECA80:"channel < arrsize(m_channels)"
00007FF69A783609 | E8 B297E0FF              | call gw2-64.7FF69A58CDC0                                   |
00007FF69A78360E | 83FB 11                  | cmp ebx,11                                                 |
00007FF69A783611 | 75 2B                    | jne gw2-64.7FF69A78363E                                    |
00007FF69A783613 | 8B87 F4080000            | mov eax,dword ptr ds:[rdi+8F4]                             |
00007FF69A783619 | 83F8 28                  | cmp eax,28                                                 | 28:'('
00007FF69A78361C | 73 13                    | jae gw2-64.7FF69A783631                                    |
00007FF69A78361E | 48:8B84C7 D8060000       | mov rax,qword ptr ds:[rdi+rax*8+6D8]                       |
00007FF69A783626 | 48:8B5C24 30             | mov rbx,qword ptr ss:[rsp+30]                              |
00007FF69A78362B | 48:83C4 20               | add rsp,20                                                 |
00007FF69A78362F | 5F                       | pop rdi                                                    |
00007FF69A783630 | C3                       | ret                                                        |
00007FF69A783631 | 33C0                     | xor eax,eax                                                |
00007FF69A783633 | 48:8B5C24 30             | mov rbx,qword ptr ss:[rsp+30]                              |
00007FF69A783638 | 48:83C4 20               | add rsp,20                                                 |
00007FF69A78363C | 5F                       | pop rdi                                                    |
00007FF69A78363D | C3                       | ret                                                        |
00007FF69A78363E | 48:8B84DF D8060000       | mov rax,qword ptr ds:[rdi+rbx*8+6D8]                       |
00007FF69A783646 | 48:8B5C24 30             | mov rbx,qword ptr ss:[rsp+30]                              |
00007FF69A78364B | 48:83C4 20               | add rsp,20                                                 |
00007FF69A78364F | 5F                       | pop rdi                                                    |
00007FF69A783650 | C3                       | ret                                                        |

00007FF69AA74810
00007FF69AA74810 | 48:895C24 20             | mov qword ptr ss:[rsp+20],rbx                              |
00007FF69AA74815 | 57                       | push rdi                                                   |
00007FF69AA74816 | 48:81EC 70020000         | sub rsp,270                                                |
00007FF69AA7481D | 48:8B05 FCA66001         | mov rax,qword ptr ds:[7FF69C07EF20]                        |
00007FF69AA74824 | 48:33C4                  | xor rax,rsp                                                |
00007FF69AA74827 | 48:898424 60020000       | mov qword ptr ss:[rsp+260],rax                             |
00007FF69AA7482F | 48:8BD9                  | mov rbx,rcx                                                |
00007FF69AA74832 | 8BFA                     | mov edi,edx                                                |
00007FF69AA74834 | 8BCA                     | mov ecx,edx                                                |
00007FF69AA74836 | E8 25E18000              | call gw2-64.7FF69B282960                                   |
00007FF69AA7483B | 8BCF                     | mov ecx,edi                                                |
00007FF69AA7483D | E8 3EBF7F00              | call gw2-64.7FF69B270780                                   |
00007FF69AA74842 | 85C0                     | test eax,eax                                               |
00007FF69AA74844 | 0F84 C9010000            | je gw2-64.7FF69AA74A13                                     | Если сообщение в чат пустое - переходим по ссылке 
00007FF69AA7484A | 48:89B424 90020000       | mov qword ptr ss:[rsp+290],rsi                             |
00007FF69AA74852 | 33F6                     | xor esi,esi                                                |
00007FF69AA74854 | 48:897424 28             | mov qword ptr ss:[rsp+28],rsi                              |
00007FF69AA74859 | 897424 30                | mov dword ptr ss:[rsp+30],esi                              |
00007FF69AA7485D | E8 3E60B0FF              | call gw2-64.7FF69A57A8A0                                   |
00007FF69AA74862 | 48:8D5424 28             | lea rdx,qword ptr ss:[rsp+28]                              |
00007FF69AA74867 | 48:C74424 38 01000000    | mov qword ptr ss:[rsp+38],1                                |
00007FF69AA74870 | 48:8BCB                  | mov rcx,rbx                                                |
00007FF69AA74873 | E8 58E7FFFF              | call gw2-64.7FF69AA72FD0                                   | в результате ссылка на сообщение, кодировка Unicode
00007FF69AA74878 | 4C:8D8424 D0000000       | lea r8,qword ptr ss:[rsp+D0]                               |
00007FF69AA74880 | 66:89B424 D0000000       | mov word ptr ss:[rsp+D0],si                                |
00007FF69AA74888 | BA C8000000              | mov edx,C8                                                 |
00007FF69AA7488D | 8BCF                     | mov ecx,edi                                                |
00007FF69AA7488F | E8 3CBD7F00              | call gw2-64.7FF69B2705D0                                   |
00007FF69AA74894 | E8 B7F1D0FF              | call gw2-64.7FF69A783A50                                   |
00007FF69AA74899 | 48:8D9424 D0000000       | lea rdx,qword ptr ss:[rsp+D0]                              |
00007FF69AA748A1 | 48:8BC8                  | mov rcx,rax                                                |
00007FF69AA748A4 | 4C:8B00                  | mov r8,qword ptr ds:[rax]                                  |
00007FF69AA748A7 | 41:FF90 B0000000         | call qword ptr ds:[r8+B0]                                  |
00007FF69AA748AE | 48:8D15 8311EF00         | lea rdx,qword ptr ds:[7FF69B965A38]                        |
00007FF69AA748B5 | 8BCF                     | mov ecx,edi                                                |
00007FF69AA748B7 | E8 84BF7F00              | call gw2-64.7FF69B270840                                   |
00007FF69AA748BC | 48:8B4C24 28             | mov rcx,qword ptr ss:[rsp+28]                              |
00007FF69AA748C1 | 48:85C9                  | test rcx,rcx                                               |
00007FF69AA748C4 | 74 2C                    | je gw2-64.7FF69AA748F2                                     |
00007FF69AA748C6 | 66:8339 2F               | cmp word ptr ds:[rcx],2F                                   | 2F:'/'
00007FF69AA748CA | 75 26                    | jne gw2-64.7FF69AA748F2                                    |
00007FF69AA748CC | 48:83C1 02               | add rcx,2                                                  |
00007FF69AA748D0 | E8 CB934700              | call gw2-64.7FF69AEEDCA0                                   |
00007FF69AA748D5 | 48:85C0                  | test rax,rax                                               |
00007FF69AA748D8 | 74 13                    | je gw2-64.7FF69AA748ED                                     |
00007FF69AA748DA | 48:8BD0                  | mov rdx,rax                                                |
00007FF69AA748DD | 48:8BCB                  | mov rcx,rbx                                                |
00007FF69AA748E0 | E8 7B0A0000              | call gw2-64.7FF69AA75360                                   |
00007FF69AA748E5 | 85C0                     | test eax,eax                                               |
00007FF69AA748E7 | 0F85 0A010000            | jne gw2-64.7FF69AA749F7                                    |
00007FF69AA748ED | 48:8B4C24 28             | mov rcx,qword ptr ss:[rsp+28]                              |
00007FF69AA748F2 | E8 A9E1D1FF              | call gw2-64.7FF69A792AA0                                   |
00007FF69AA748F7 | 85C0                     | test eax,eax                                               |
00007FF69AA748F9 | 0F85 F8000000            | jne gw2-64.7FF69AA749F7                                    |
00007FF69AA748FF | 8B4B 18                  | mov ecx,dword ptr ds:[rbx+18]                              |
00007FF69AA74902 | 8D50 01                  | lea edx,qword ptr ds:[rax+1]                               |
00007FF69AA74905 | E8 36D38000              | call gw2-64.7FF69B281C40                                   |
00007FF69AA7490A | 85C0                     | test eax,eax                                               |
00007FF69AA7490C | 74 4C                    | je gw2-64.7FF69AA7495A                                     |
00007FF69AA7490E | 4C:8D4424 40             | lea r8,qword ptr ss:[rsp+40]                               |
00007FF69AA74913 | 66:897424 40             | mov word ptr ss:[rsp+40],si                                |
00007FF69AA74918 | BA 48000000              | mov edx,48                                                 | 48:'H'
00007FF69AA7491D | 8BC8                     | mov ecx,eax                                                |
00007FF69AA7491F | E8 3CBD7F00              | call gw2-64.7FF69B270660                                   |
00007FF69AA74924 | 8B4B 18                  | mov ecx,dword ptr ds:[rbx+18]                              |
00007FF69AA74927 | BA 01000000              | mov edx,1                                                  |
00007FF69AA7492C | E8 0FD38000              | call gw2-64.7FF69B281C40                                   |
00007FF69AA74931 | 85C0                     | test eax,eax                                               |
00007FF69AA74933 | 74 07                    | je gw2-64.7FF69AA7493C                                     |
00007FF69AA74935 | 8BC8                     | mov ecx,eax                                                |
00007FF69AA74937 | E8 94CC8000              | call gw2-64.7FF69B2815D0                                   |
00007FF69AA7493C | E8 0FF1D0FF              | call gw2-64.7FF69A783A50                                   |
00007FF69AA74941 | 4C:8B4424 28             | mov r8,qword ptr ss:[rsp+28]                               |
00007FF69AA74946 | 48:8D5424 40             | lea rdx,qword ptr ss:[rsp+40]                              |
00007FF69AA7494B | 48:8BC8                  | mov rcx,rax                                                |
00007FF69AA7494E | 4C:8B08                  | mov r9,qword ptr ds:[rax]                                  |
00007FF69AA74951 | 41:FF51 48               | call qword ptr ds:[r9+48]                                  |
00007FF69AA74955 | E9 9D000000              | jmp gw2-64.7FF69AA749F7                                    |
00007FF69AA7495A | E8 F1F0D0FF              | call gw2-64.7FF69A783A50                                   |
00007FF69AA7495F | 48:8BC8                  | mov rcx,rax                                                |
00007FF69AA74962 | 48:8B10                  | mov rdx,qword ptr ds:[rax]                                 |
00007FF69AA74965 | FF92 10010000            | call qword ptr ds:[rdx+110]                                |
00007FF69AA7496B | 48:8BD8                  | mov rbx,rax                                                |
00007FF69AA7496E | 48:85C0                  | test rax,rax                                               |
00007FF69AA74971 | 75 34                    | jne gw2-64.7FF69AA749A7                                    |
00007FF69AA74973 | E8 D8F0D0FF              | call gw2-64.7FF69A783A50                                   |
00007FF69AA74978 | 48:8BC8                  | mov rcx,rax                                                |
00007FF69AA7497B | 48:8B10                  | mov rdx,qword ptr ds:[rax]                                 |
00007FF69AA7497E | FF92 18010000            | call qword ptr ds:[rdx+118]                                |
00007FF69AA74984 | 8BC8                     | mov ecx,eax                                                |
00007FF69AA74986 | E8 C5E8FFFF              | call gw2-64.7FF69AA73250                                   |
00007FF69AA7498B | 48:8BD8                  | mov rbx,rax                                                |
00007FF69AA7498E | E8 BDF0D0FF              | call gw2-64.7FF69A783A50                                   |
00007FF69AA74993 | 4C:8BC3                  | mov r8,rbx                                                 |
00007FF69AA74996 | BA 0E000000              | mov edx,E                                                  |
00007FF69AA7499B | 48:8BC8                  | mov rcx,rax                                                |
00007FF69AA7499E | 4C:8B08                  | mov r9,qword ptr ds:[rax]                                  |
00007FF69AA749A1 | 41:FF51 70               | call qword ptr ds:[r9+70]                                  |
00007FF69AA749A5 | EB 50                    | jmp gw2-64.7FF69AA749F7                                    |
00007FF69AA749A7 | 48:8B00                  | mov rax,qword ptr ds:[rax]                                 |
00007FF69AA749AA | 48:8BCB                  | mov rcx,rbx                                                |
00007FF69AA749AD | FF50 30                  | call qword ptr ds:[rax+30]                                 |
00007FF69AA749B0 | 85C0                     | test eax,eax                                               |
00007FF69AA749B2 | 48:8BCB                  | mov rcx,rbx                                                |
00007FF69AA749B5 | 48:8B03                  | mov rax,qword ptr ds:[rbx]                                 |
00007FF69AA749B8 | 75 35                    | jne gw2-64.7FF69AA749EF                                    |
00007FF69AA749BA | 48:8D5424 20             | lea rdx,qword ptr ss:[rsp+20]                              |
00007FF69AA749BF | 897424 20                | mov dword ptr ss:[rsp+20],esi                              |
00007FF69AA749C3 | FF50 40                  | call qword ptr ds:[rax+40]                                 |
00007FF69AA749C6 | 48:8BD8                  | mov rbx,rax                                                |
00007FF69AA749C9 | 397424 20                | cmp dword ptr ss:[rsp+20],esi                              |
00007FF69AA749CD | 74 BF                    | je gw2-64.7FF69AA7498E                                     |
00007FF69AA749CF | E8 EC1E8600              | call gw2-64.7FF69B2D68C0                                   |
00007FF69AA749D4 | 85C0                     | test eax,eax                                               |
00007FF69AA749D6 | 74 B6                    | je gw2-64.7FF69AA7498E                                     |
00007FF69AA749D8 | E8 73F0D0FF              | call gw2-64.7FF69A783A50                                   |
00007FF69AA749DD | 48:8BD3                  | mov rdx,rbx                                                |
00007FF69AA749E0 | 48:8BC8                  | mov rcx,rax                                                |
00007FF69AA749E3 | 4C:8B00                  | mov r8,qword ptr ds:[rax]                                  |
00007FF69AA749E6 | 41:FF90 B0020000         | call qword ptr ds:[r8+2B0]                                 |
00007FF69AA749ED | EB 08                    | jmp gw2-64.7FF69AA749F7                                    |
00007FF69AA749EF | 48:8B5424 28             | mov rdx,qword ptr ss:[rsp+28]                              |
00007FF69AA749F4 | FF50 58                  | call qword ptr ds:[rax+58]                                 |
00007FF69AA749F7 | 48:8B4C24 28             | mov rcx,qword ptr ss:[rsp+28]                              |
00007FF69AA749FC | 48:8BB424 90020000       | mov rsi,qword ptr ss:[rsp+290]                             |
00007FF69AA74A04 | 48:85C9                  | test rcx,rcx                                               |
00007FF69AA74A07 | 74 0A                    | je gw2-64.7FF69AA74A13                                     |
00007FF69AA74A09 | 45:33C0                  | xor r8d,r8d                                                |
00007FF69AA74A0C | 33D2                     | xor edx,edx                                                |
00007FF69AA74A0E | E8 1DCDB0FF              | call gw2-64.7FF69A581730                                   |
00007FF69AA74A13 | 48:8B8C24 60020000       | mov rcx,qword ptr ss:[rsp+260]                             |
00007FF69AA74A1B | 48:33CC                  | xor rcx,rsp                                                |
00007FF69AA74A1E | E8 EDF1C8FF              | call gw2-64.7FF69A703C10                                   |
00007FF69AA74A23 | 48:8B9C24 98020000       | mov rbx,qword ptr ss:[rsp+298]                             |
00007FF69AA74A2B | 48:81C4 70020000         | add rsp,270                                                |
00007FF69AA74A32 | 5F                       | pop rdi                                                    |
00007FF69AA74A33 | C3                       | ret                                                        |
```
