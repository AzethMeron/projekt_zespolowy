# Polish project. I consider this one a failure, tho I learned good lesson from it.

PWr W4 Projekt zespolowy 2020

M.Pietras, J.Grzana

Projekt: wyszukiwanie osób na podstawie portretów pamięciowych

Ostateczne wersje programów znajdują się w katalogu SerwerKlient


Historia postępów:

17.03.2020: M.Pietras

Dodano programy, które umożliwiają określenie podobieństwa wybranej osoby do osób, które znajdują się już w bazie danych.

Folder images - zamiera bazę danych (dataset) z kilkoma sławnymi aktorami.

Potrtret pamięciowy (images/nicolas_cage_test/nc1)- zdjęcie Nicolasa Cage'a w kapeluszu i brodzie (w odróżnieniu od obrazów w bazie).

encodings.py - program tworzący plik z kodowaniem twarzy znajdujących się w bazie danych. Przykład wywołania: python encodings.py -d 
images/dataset -e encodings/facial_encodings.pkl   

calculations.py - program obliczający podobieństwo danej osoby do portretu pamięciowego. Przykład wywołania: python calculations.py -i images/nicolas_cage_test/nc1.jpg -e encodings/facial_encodings.pkl   

Na postawie: https://medium.com/swlh/facial-recognition-a-visual-step-by-step-d679289bab11


08.04.2020: M.Pietras

Sprawdzono bazy danych zdjęć wymienione na stronie https://www.face-rec.org/databases/. Większość z nich jest możliwa do pobrania w postaci archiwum. 

Dodano program data_download.py, który pobiera bazę danych ze zdjęciami po podaniu adresu url i folderu zapisu.

29.04.2020: J.Grzana

Trzecia rewizja programu Klient-Serwer, napisanych z użyciem SFMLa oraz wątków Pthread. 

Aplikacja kliencka jest przenośna, wymaga SFMLa który jest jednak dostępny na większość platform. Umożliwia wybór IP serwera z którym się łączy, oraz pliku graficznego do przesłania. Nawiązuje połączenie na określonych w kodzie portach, przesyła plik, odbiera odpowiedź i wyświetla ją na ekranie. 

Aplikacja serwera nasłuchuje na porcie PORT_CON (obecnie 32000) i po nawiązaniu łączności przydziela klientowi wątek i port. Klient łaczy się z podanym portem, gdzie zostaje "obsłużony". W tej chwili działa wszystko (łącznie z przesyłem danych, przetestowane) oprócz wywołania skryptów Pythonowych, umożliwiających rozpoznawanie twarzy. Zalążek tej części jest, ale niedokończony.

Katalog BazaDanych zawiera foldery z informacjami o osobach w postaci zdjęć oraz pliku info.txt. 3 pierwsze linijki tego pliku powinny zawierać: imie, nazwisko, pesel. Dalsza czesc to inne notatki. Nie ma limitów długości, wymagane jest kodowanie ASCII. 

18.05.2020: J. Grzana

Czwarta rewizja programu Klient-Serwer.
Funkcja CallScript działa, ale tylko raz. Przy drugim wywołaniu powoduje błąd Pythona. 

22.05.2020: J. Grzana

Próbowałem dodać interpreter Pythona do pliku wykonywalnego, ale szybko się okazało że to nie jest rozwiązanie problemu. Mało jest w internecie informacji dotyczących występującego u nas błędu. Ponadto, okazało się że Python w C++ nie jest przystosowany do wielowątkowosci więc program straciłby jedną ze swoich wielkich zalet - obsługę wielu klientóœ jednocześnie. Doszedłem do wniosku, że wykorzystanie wątków Pthread było błędem - należało użyć modelu procesów. Jak widać tutaj: https://stackoverflow.com/questions/2968317/multiple-independent-embedded-python-interpreters-on-multiple-operating-system-t ludzie bardziej doświadczeni z zagnieżdżaniem Pythona również sugerują procesy. Na tym etapie wprowadzenie tej zmiany byłoby jednak zbyt pracochłonne.
