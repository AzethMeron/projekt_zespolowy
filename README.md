PWr W4 Projekt zespolowy 2020

M.Pietras, J.Grzana

Projekt: wyszukiwanie osób na podstawie portretów pamięciowych


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
