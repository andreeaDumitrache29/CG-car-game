DUMITRACHE DANIELA ANDREEA
Elemente de Grafica pe Calculator - Tema2
331CB

Am implementat tema pornind de la laboratoarele 5 (de unde am
luat camera) si 6 (de unde am luat fisierele de VertexShader si
FragmentShader).

Masina consta intr-un paralelipiped pentru caroserie si 4
cilindri pentru roti. Cilindrii sunt obj-uri luate de pe
internet, iar paralelipipedul a fost creat folosind functia
CreateParallelepiped. Miscarea se face in sensul pozitiv al
axei Oz. Rotile au doua unghiuri cu care se rotesc: unul
pentru a simula miscarea lor in jurul centrului propriu
si unul pentru rotatia masinii dupa directia de deplasare.
Exista o viteza maxima pana la care poate accelera masina.
Daca in urma franarii viteza ajunge 0 si se continua apasarea
tastei sageat jos atunci masina se va deplasa cu spatele.

Cerul este o sfera scalata cu un factor de 400 si care se
translateaza odata cu masina.
Pamantul este un dreptunghi scalat cu un factor de 300 care se
translateaza si el odata cu masina.
Traseul consta in 5 portiuni de drum, fiecare creata folosind
un dreptunghi format din 2 triunghiuri. Fiecare portiune este
marginita de borduri create folosind paralelipipede.

Pentru a simula tranzitia zi-noapte am definit 4 momente ale
zilei: dimineata, dupa-amiaza, apus si noapte. Putem afla
in ce moment al zilei de aflam folosind sin si cos:
dimineata = cadranul 1 => sin pozitiv si cos pozitiv
dupa-amiaza = cadranul 2 => sin pozitiv si cos negativ
apus = cadranul 3 => sin negativ si cos negativ
noapte = cadran 4 => sin negativ si cos pozitiv

Astfel, cerul, pamantul si drumul au cate o culoare definita
pentru fiecare moment al zilei. Vertex shader primeste ca
uniforme culoarea obiectului din momentul actual, culoarea din
urmatorul stagiu al zilei si timpul curent si se realizeaza 
tranzitia treptata de la un moment la altul prin interpolare.

Pentru camerele first person si third person am creat 2 camere
separate care se misca odata cu masina si se face switch intre
ele la apasarea unei tastei. Camera first person se afla pe
botul masinii, aceasta nefiind vizibila cand acest mod este
activat. Camera third person priveste masina din spate
si o urmareste constand, executand aceleasi miscari ca si masina.

Pentru desenarea cadrului in format wireframe se foloseste
polygonMode.

Exista cate un obstacol pe fiecare portiune de drum, create 
folosind parelipipede. Coliziunea dintre masina si obstacole sau
borduri este determinata pe baza coliziunii dintre 2 dreptunghiuri.
Dreptunghiurile comparate vor fi cel de la baza
masinii si cel de la baza paralelipipedelor. Pentru a sti mereu
coordonatele punctelor care formeaza aceste dreptunghiuri
retin intr-un vector coordonatele cu care sunt create masina si
paralelipipedul, apoi la fiecare pas calculez noile coordonate
prin inmultirea cu modelMatrix.
Atunci cand masina intra in coliziune cu bordurile aceasta se
opreste, iar jucatorul trebuie sa o repozitioneaze pentru a 
continua traseul.
Cand masina intra in coliziune cu obstacolele se opreste, iar
obstacolele dispar pentru a usura revenirea pe traseu a
jucatorului.

Jucatorul acumuleaza puncte prin deplasarea pe sosea si are la
dispozitie 4 vieti. Cand se ajunge a patra coliziune jocul se
incheie. Daca jucatorul ajunge la linia de finish cu un numar
pozitv de vieti, atunci el a castigat si se afiseaza la consola
un mesaj prin care este informat de scorul sau. Scorul se scade
cu o treime la coliziunea cu bordurile si cu jumatate la
coliziunea cu un obstacol.

Taste folosite:
- miscare inainte si accelerare: sageata sus
- mers cu spatele si franare: sageata jos
- rotatie stanga: sageata stanga
- rotatie dreapta: sageata dreapta
- schimbare camera: T
- desenare wireframe / fill: L