Nume: Nuță Mihaela-Mădăline
Grupă: 334CB

# Tema 2 SO

## Soluția de ansamblu:
* Soluția de ansamblu este bazată pe folosirea funcțiilor prezente în biblioteca `fcntl.h` pentru a implementa o parte din funcționalitatea bibliotecii `stdio.h`. 
* În cadrul temei este folosită o structură de date SO_FILE care conține un buffer, un offset intern de citire/scriere, file-descriptor, ultima operație efectuată asupra fișierului, cât și alte variabile de stare care reprezintă starea curentă a fișierului.
* La nivel general, funcțiile pot fi descrise în felul următor:
	* **so_fopen**: folosește apelul `open`  pentru a deschide un fișier în modul precizat ca și parametru al funcției
	* **so_fclose**: se folosește de `so_fflush` pentru a scrie bufferul în fișier, după care închide fișierul reprezentat de file-descriptor cu apelul `close`
	* **so_fgetc**: folosește apelul `read` și scrie într-un buffer. Funcția are grijă de cazul în care apelul nu întoarce numărul de bytes specificat ca și argument, iar la următorul apel de citire, în cazul în care s-a citit tot bufferul până la acel index, funcția va citi doar cât mai are liber în buffer. Adică, daca **offsetul intern** al bufferului a ajuns la indexul **i**, iar în buffer au fost citiți doar *i* octeți, urmărul apel de read va încerca să umple bufferul, nu să îl reinițializeze. Funcția returneaza caracterul care se află la poziția capului curent de citire
	*  **so_fileno**:  returnează file-descriptorul structurii de date *SO_FILE*
	* **so_fread**: se folosește de funcția deja implementată `so_fgetc`. Se încearcă citirea a mai multor blocuri de dimensiune setată folosind 2 *foruri* imbricate și salvarea datelor cititire in bufferul primit ca parametru
	* **so_fputc**: folosește apelul `write` prin intermediul funcției *so_fflush*. Cât timp bufferul nu este plin, funcția pur și simplu salvează datele în buffer. Când bufferul se umple. el este scris în fișier și reinițializat. Dacă înainte, asupra fișierului a fost făcută o  operație de tip citire, se repoziționează offsetul fișierului și bufferul este reinițializat
	* **so_fwrite**: la fel ca și *so_fread*, cu 2 foruri imbricate se folosește de funcția deja implementată `so_fputc`
	* **so_fflush**: se folosește de offsetul intern al bufferului și scrie cu ajutorul apelului `write` într-o buclă, pentru a acoperi cazul în care apelul scrie mai puțin decât i s-a cerut
	* **so_ftell**: se poziționeaza la offsetul real cu ajutorul `so_fseek` și se foloseste de apelul `lseek` pentru a returna offsetul real al fișierului: apelat cu offset = 0 și whence = SEEK_CUR, apelul returnează offsetul curent.
	* **so_fseek**: se folosește de apelul `lseek`
	* **so_feof**: returneaza variabila de stare a structurii de fișier deschisă, variabilă care este modificată de-a lungul programului în cazul în care s-a ajuns la END-OF-FILE
	* **so_ferror**: returneaza variabila de stare a structurii de fișier deschisă care este modificată de-a lungul programului în cazul în care apare o eroare
>**Utilitatea temei:**

> Consider că tema a contribuit foarte mult la înțelegerea funcțiilor incluse în biblioteca stdio.h, înțelegerea importanței codurilor de eroare care se pot propaga între funcții, cât și la înțelegerea mai bună a diferențelor dintre `fread-read` / `fwrite-write` / `fopen-open` etc. 

## Implementare
Tema nu implementeaza partea legată de popen, iar acest lucru se observă la nivel de teste.
* Cazuri speciale care au fost tratate:
	* la nivel de citire, bufferul este in continuă creștere până la lungimea maximă, în cazul în care apelul `read` nu întoarce exact cât i s-a cerut
	* la nivel de scriere, so_fflush se ocupa de cazul în care apelul `write` nu scrie exact cât i s-a cerut, ci mai puțin
* Dificultăți întâmpinate:
	* dificultatea în a găsi resurse unde se explică cu exactitate cum funcționeaza buffering-ul împreună cu apelurile fseek/ftell
* Lucruri interesante descoperite pe parcurs
	* cât de multă informație utilă se găseste în ``man``
	* faptul că poți implementa si folosi propria librărie de input/output

## Cum se compilează și cum se rulează?
- Build: `make` sau `make build` -> se crează o bibliotecă dinamică `libso_stdio.so`

Bibliografie
-
- a lot of **`man`**