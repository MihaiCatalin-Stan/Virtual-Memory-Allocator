**Nume:Stan Mihai-Catalin**
**315CA**

## VMA Tema1

## Descriere:

# Mod de functionare
	Programul are la baza o serie de structuri imbricate folosite pentru
implementarea cerintelor date.
1. arena_t - Structura de arena, ce va stoca in aceasta:
	1.1 O lista a caror noduri vor retine blocurile alocate in memorie.
	1.2 Un size ce va retine numarul de bytes al arenei. (Atat alocati cat
	si liberi)
2. list_t - O lista dublu inlantuita generica, ce va fi folosita pentru a stoca
	atat blocuri cat si miniblocuri. Lista contine
	2.1 Doi pointeri de tipul node_t, unul ce va pointa catre inceputul
	listei si unul ce va pointa catre finalul listei, pentru usurinta
	adaugarii/eliminarii datelor din aceasta.
	2.2 Un size ce va retine numarul de noduri asociate listei.
3. node_t - O structura de tipul nod, creata pentru a facilita implementarea
	listei dublu inlantuite. Aceasta contine:
	3.1 Un pointer de tipul void ce va fi alocat dinamic si va retine orice
	tip de data se doreste a fi pus in nod. (in cazul de fata blouri si
	miniblocuri)
	3.2 Doi pointeri de tipul node_t, unul ce va pointa catre urmatorul nod
	din lista si unul ce va pointa catre nodul precedent din lista.
4. block_t - O structura ce va simula un bloc din memorie, si in care cu
	ajutorul listelor vor fi stocate miniblocurile corespunzatoare. Aceasta
	structura contine:
	4.1 O adresa de start de la care va fi alocat in arena data.
	4.2 O dimensiune pe care o va ocupa in arena.
	4.3 O lista ce va retine miniblocurile asociate acestei zone din
	memoria virtuala.
5. miniblock_t - O structura ce va simula un miniblock din memorie, si care va
	fi stocat in lista de miniblocuri a structurii de tip block_t. Aceasta
	structura contine:
	5.1 O adresa de start de la care va incepe miniblocul.
	5.2 Dimensiunea in bytes pe care o va ocupa in blocul asociat.
	5.3 Un numar asociat permisiunilor in octal. (R = 4 ; W = 2 ; X = 1)
	5.4 Un buffer de tipul void * care se va aloca corespunzator size-ului
	miniblocului, si in care vor fi scrise (sau din care vor fi citite)
	date primite de la stdin.


# main
	Main-ul programului are implementat cu ajutorul unei structuri
repetitive cu test final si al mai multor structuri de decizie, o serie de
apeluri catre functiile corespunzatoare unor input-uri.
	Aici se va declara arena si se vor citi de la tastatura comenzile si
daca este nevoie datele necesare executarii acestora (size/address/write_texts)


# Implementarea listelor

# list_create
	Functie ce primeste ca parametru dimensiunea elementelor din lista si
initializeaza o structura de tip list_t ce va retine noduri cu elemente de
dimensiunea dorita.
	Functia returneaza lista initializata.
	
# add_node
	Functie ce primeste ca parametrii lista in care se doreste a se face
adaugarea, pozitia pe care se doreste a se adauga elementul si informatia ce se
doreste a se retine in nod.
	Aceasta nu returneaza nimic si face modificarile direct pe lista.
	De asemenea, abordeaza separat cazurile pentru primul nod din lista,
ultimul nod din lista si singurul nod din lista.

# remove_node
	Functie ce primeste ca parametrii lista din care se doreste a fi scos
un nod cat si pozitia de pe care se doreste a fi facuta aceasta operatie.
	Functia abordeaza pe rand cazurile pentru ultimul nod (in cazul unei
liste cu minim 2 elemente), primul nod si singurul nod, iar in final cazul
general pentru oricare alt nod de pe pozitia n.
	Functia returneaza nodul ce se doreste a fi eliminat.

# list_free
	Functie ce primeste ca parametru lista pentru care se doreste a se face
free. Aceasta se parcurge si se elimina mereu primul nod din lista, se muta
head-ul listei, se da free la informatia din nod, apoi la nod si se
decrementeaza dimensiunea listei. In final se da free la structura de lista
care nu mai are niciun nod in aceasta.


# Implementarea arenei

# alloc_arena
	Functie ce primeste ca parametru dimensiunea in bytes a arenei si
returneaza arena in urma alocarii acesteia.
	Se aloca structura pentru arena, (si se verifica daca aceasta s-a
alocat cu succes) se seteaza dimensiunea arenei si se initializeaza cu ajutorul
functiei de list_create, o lista in care a caror noduri se vor salva structuri
de tipul block_t.

# dealloc_arena
	Functie ce primeste ca parametru arena si elibereaza spatiul alocat de
aceasta si de structurile adiacente sale din memorie.
	Se seteaza un cursor de tipul node_t la inceputul listei de blocuri si
parcurge blocurile din lista de blocuri a arenei.
	Pentru fiecare bloc se parcurg miniblocurile si se elibereaza buffer-ul
pentru functiile de read/write al fiecarui minibloc. La finalul parcurgerii
unui bloc, se elibereaza din memorie lista de miniblocuri salvata in structura
block_t (adica miniblocurile din lista, nodurile care pointeaza catre acestea
si lista in sine).
	Blocul va ramane alocat pana la finalul parcurgerii, cand se va elibera
din memorie cu ajutorul functiei de list_free. Aceasta va elibera blocurile din
campul alloc_list al arenei, nodurile in care sunt salvate blocurile si lista
propriu-zisa. In final se va elibera arena.


# Implementarea blocurilor

# alloc_block
	Functie ce primeste ca parametrii arena in care se doreste a se aloca
noul miniblock,(eventual noul block in anumite cazuri) adresa de start a
acestuia si size-ul. Schimbarile se vor face direct pe arena, iar functia nu va
returna nimic.
	Initial se verifica daca ne aflam intr-un caz de eroare cu ajutorul
functiei de 'alloc_errors'. In cazul in care alocarea se face intr-un spatiu
valid, vom alocat noul miniblock cu ajutorul functiei de alloc_miniblock.
	Apoi se vor verifica si adauga miniblocuri/blocuri pe rand dupa
urmatoarele cazuri:
1. Cand lista de blocuri este goala. Initializam un nou bloc cu functia de
alloc-in-func-block, adaugam miniblocul creat in acesta, si apoi blocul nou in
lista arenei.
	Ulterior parcurgem blocurile existente si adaugam dupa cazurile:
2. Daca miniblocul nou trebuie contopit cu adresa de inceput a blocului curent,
se va adauga noul miniblock la inceputul listei blocului curent.
2.1 Daca s-a efectuat contopirea cu blocul curent, se verifica daca noul
miniblock va contopi blocul curent cu cel anterior, caz in care se vor crea
legaturi intre miniblocurile acestora si se va elibera blocul curent.
3. Adaugam miniblocul nou intr-un nou bloc inaintea blocului curent
4. In cazul in care am ajuns pe ultimul bloc din lista, vom putea:
4.1 Adauga la finalul listei
4.2 Contopeste miniblocul nou la finalul ultimului bloc din lista.
5. Daca miniblocul curent trebuie contopit cu finalul blocului curent, dar nu
si cu inceputul blocului urmator, se va face o adaugare asemeni celei de la 4.2
Notes: Explicatia pe scurt in comentariile din cod

# alloc_errors
	Functie de tipul int care returneaza 0 sau 1 in urma verificarii
cazurilor de eroare pentru functia de alloc_block. Daca blocul ce se doreste a
fi alocat incepe sau se termina in afara spatiului disponibil al arenei, ori in
cazul in care zona pe care incercam sa o alocam este deja alocata altui bloc,
functia va afisa un mesaj, va returna 1, iar functia de alloc_block va fi
intrerupta.

# alloc_miniblock
	Functie ce primeste ca parametrii arena, size-ul miniblocului nou si
adresa de inceput a acestuia. Functia declara si initializeaza un miniblock si
campurile acesteia, verifica daca a fost creat cu succes (in caz contrat arena
si structurile asociate sunt eliberate, iar programul este intrerupt), si
returneaza noul miniblock

# alloc_in_func_block
	Functie ce primeste ca parametrii arena, size-ul noului bloc si adresa
de inceput a acestuia. Functia declara si initalizeaza un block si campurile
adiacente acestuia, inclusiv lista pentru miniblocuri, creata cu functia de
list_create. In cazul in care alocarea blocului esueaza, arena este eliberata
din memoria (la fel si restul de date din aceasta), si programul este intrerupt
in caz contrat, se va returna noul block.

# free_block
	Functie ce primeste ca parametrii arena si adresa miniblocului ce se
doreste a fi dealocat. In functie parcurgem blocurile pe rand. In cazul in care
adresa cautata se afla in blocul curent, vom parcurge miniblocurile acestuia.
Aici se vor verifica urmatoarele cazuri:
1. Cazul in care exista doar un minibloc in bloc. Aici se elibereaza buffer-ul,
miniblocul, nodul in care este salvat, lista de miniblocuri si blocul curent.
2. In cazul eliminarii primului miniblock dintr-un block. Ne vom folosi de
functiile de la liste si vom returna nodul ce contine miniblocul ce trebuie
dealocat.
3. Cazul eliminarii ultimului miniblock dintr-un block. Functioneaza similar cu
cazul eliminarii primului miniblock.
4. Cazul eliminarii unui miniblock ce se afla intre alte doua miniblocuri. Aici
se vor reface legaturile din lista de miniblocuri a blocului curent. Apoi se va
crea un bloc nou, iar nodurile din dreapta (cu adresa mai mare) decat a
miniblocului eliminat vor fi salvate intr-un nou block. Blocul curent va fi
redimensionat corespunzator.

# Implentare functii & permisiuni

# read
	Functie ce primeste ca parametrii arena, adresa la care se doreste a fi
citita informatia si numarul de bytes ce trebuie citit.
	Parcurgem blocurile pana ajungem la cel care contine adresa cautata,
apoi parcurgem miniblocurile cu acelasi scop. Setam un offset in cazul in care
adresa nu este cea de inceput a miniblocului, si vom citi din acesta un numar
dat de bytes
1. In cazul in care se doresc a fi cititi mai putini bytes decat dimensiunea
miniblocului.
2. In cazul in care se doresc a fi cititi toti bytes din miniblocul curent.
	Acesti bytes sunt adaugati intr-un string care va fi afisat la finalul
subprogramului.

# write
	Functie ce primeste ca parametrii arena, adresa la care se doreste a fi
citita informatia, numarul de bytes ce trebuie scrisi si mesajul cerut.
	Parcurgem blocurile pana ajungem la cel care contine adresa cautata,
apoi parcurgem miniblocurile ca in functia de read. Setam un offset de la care
sa scriem mesajul in minibloc (dupa acelasi principiu ca in functia de read).
	Scriem un numar dat de bytes dupa caz:
1. Daca se doresc a fi scrisi mai multi bytes decat dimensiunea miniblocului,
atunci vom scrie doar diferenta dintre size-ul miniblocului si offset-ul
calculat (conditia de pe else)
2. Daca se doresc a fi scrisi mai putini bytes decat dimensiunea miniblocului,
atunci vom scrie doar numarul de bytes cerut

# turn_to_char_perms
	Functie ce primeste ca parametrii valoarea permisiunilor unui miniblock
in octal si un string declarat cu valoarea "---", si modfiica valoarea acestui
string pe baza permisiunilor.

# pmap
	Functie ce primeste ca parametrii arena si afiseza datele despre
blocurile si miniblocurile stocate in memorie, dimensiunea lor si permisiunile
aferente.

# mprotect
	Functie ce primeste ca parametrii arena, adresa miniblocului a carui
permisiuni trebuie schimbate si permisiunile ce trebuiesc adaugate sub forma
unui string. Functia nu returneaza nimic si schimba valoarea in octal a
permisiunilor miniblocului modificat.

# Ce am invatat
	Probabil cel mai important lucru pe care l-am inteles din tema data e
importanta comentariilor si a spatiilor in cod. VMA a fost prima tema la care
m-am pierdut prin liniile de cod lungi care refac legaturile intre blocuri si
prin secventele de cod care nu au legatura intre ele (dealocarea unui nod si
adaugarea unui nod nou).
	De asemenea am inteles importanta variabilelor locale cu care am
parcurs structurile, fara ele fiind din nou foarte pierdut prin multe sageti
si cast-uri explicite facute listelor generice.
	Cat despre codul propriu-zis, initalizarea string-urilor cu calloc e
o practica pe care nu o sa o uit prea curand. Multumim pentru hint-ul de pe
forum pentru testul 34 :)

#Snorlax wishes you all an easy day!

⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣶⣿⣶⣦⣄⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣤⣶⣾⣿⣿⣷⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⠿⠿⠿⣿⣿⣿⣿⠿⠿⠿⢿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⢀⡀⣄⠀⠀⠀⠀⠀⠀⠀⣿⣿⠟⠉⠀⢀⣀⠀⠀⠈⠉⠀⠀⣀⣀⠀⠀⠙⢿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⣀⣶⣿⣿⣿⣾⣇⠀⠀⠀⠀⢀⣿⠃⠀⠀⠀⠀⢀⣀⡀⠀⠀⠀⣀⡀⠀⠀⠀⠀⠀⠹⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⢻⣿⣿⣿⣿⣿⣿⣷⣄⠀⠀⣼⡏⠀⠀⠀⣀⣀⣉⠉⠩⠭⠭⠭⠥⠤⢀⣀⣀⠀⠀⠀⢻⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⣸⣿⣿⣿⣿⣿⣿⣿⣿⣷⣄⣿⠷⠒⠋⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠑⠒⠼⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⢹⣿⣿⣿⣿⣿⣿⣿⣿⡿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠳⣦⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⡿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢿⣷⣦⣀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠈⣿⣿⣿⣿⣿⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⢹⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀
⠀⠀⠀⠀⠀⣿⣿⣿⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣾⣿⣿⣿⣿⣿⣿⣿⣧⡀⠀⠀
⠀⠀⠀⠀⢠⣿⣿⣿⣿⣿⣶⣤⣄⣠⣤⣤⣶⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣶⣶⣶⣶⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠀⠀
⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀
⠀⠀⣀⠀⢸⡿⠿⣿⡿⠋⠉⠛⠻⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠉⠀⠻⠿⠟⠉⢙⣿⣿⣿⣿⣿⣿⡇
⠀⠀⢿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠁⠀⠀⠀⠀⠀⠀⠀⠈⠻⠿⢿⡿⣿⠳⠀
⠀⠀⡞⠛⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣇⡀⠀⠀
⢀⣸⣀⡀⠀⠀⠀⠀⣠⣴⣾⣿⣷⣆⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⣰⣿⣿⣿⣿⣷⣦⠀⠀⠀⠀⢿⣿⠿⠃⠀
⠘⢿⡿⠃⠀⠀⠀⣸⣿⣿⣿⣿⣿⡿⢀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡀⢻⣿⣿⣿⣿⣿⣿⠂⠀⠀⠀⡸⠁⠀⠀⠀
⠀⠀⠳⣄⠀⠀⠀⠹⣿⣿⣿⡿⠛⣠⠾⠿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠿⠿⠿⠳⣄⠙⠛⠿⠿⠛⠉⠀⠀⣀⠜⠁⠀⠀⠀⠀
⠀⠀⠀⠈⠑⠢⠤⠤⠬⠭⠥⠖⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠒⠢⠤⠤⠤⠒⠊⠁⠀⠀⠀⠀⠀⠀
