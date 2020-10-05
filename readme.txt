-- Credits --

Proggrammeret af Mikkel Danielsen, s183913
Github: https://github.com/MIk1812/Operativ1

-- Compilering og eksekvering --

1. Åben terminalen
2. Tjek om gcc (GNU Compiler Collection) er installeret ved at skrive "gcc –v"
    2.a Såfremt gcc ikke er installet, kan det installeres vha. følgende kommando: "sudo apt-get install build-essential"
3. Naviger til projektets rod vha. cd
4. Compilér programet med følgende kommando: "sudo gcc main.c"
5. Kør nu programet ved følgende kommando: "./a.out"

-- Manual --

Programet starter alle nye linjer med cwd (current working directory) efterfulgt af "$", hvorefter en enkelt kommando kan indtastes og afsluttes med enter.
Systemets filstruktur kan navigeres vha. "cd" kommandoen: "cd PATH" ændre cwd til PATH, "cd .." går en mappe tilbage og "cd" går direkte til home directory. Til sidst kan programmet afsluttes ved at kalde "exit".
Bemærk programmet er designet til at modtage kommandoer på maks 99 karakterer indeholdende maks 10 parametre. Desuden kan programmet kun håndtere én pipe pr. kommando, f.eks. "ls | grep Make".
Det skal også bemærkes, at programmet ikke udfører inputvalidering, dvs. indtastes ugyldige kommandoer kan uventet adfærd forekomme.
Ellers er det med programmet muligt at eksekvere standard unix command-line interface programmer såsom "ls", "pwd", "sort", "grep" osv.
Programmet skal køres på et UNIX baseret operativsystem.

-- System Calls --

Et systemkald er, som navnet antyder, kald til systemet om at udføre en opgave. Det afleverede program benytter sig i høj grad af systemkald, til at udføre sin funktionalitet via forskellige C-libaries der anvender TRAP funktionen. De relevante parametre pushes til stack'en og den korrekte library procedure kaldes som sørger for at placere det aktuelle system-kald nummer hvor operativsystemet forventer det, således at det senere kan identificeres hvilket systemkald der er blevet foretaget. Libary proceduren eksekverer da TRAP funktionen, der skifter fra user-mode til kernel mode og starter den korrekte system-call handler, der som regel identificeres vha en table over pointers til system‐call handlers indexeret over system‐call numbers. Når system-kaldet er færdigt, skiftes tilbage til user-mode og programet (caller'en) får lov at fortsætte, efter den har ryddet op i stack'en. De systemkald der bliver anvendt i programmet er som følger: fork, exec, dup2, pipe  og wait. Hvad de forskellige kald gør og hvorfor de er blevet anvendt, er forklarer nærmere i kildekoden.

