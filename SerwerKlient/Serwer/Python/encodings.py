from imutils import paths
import face_recognition
import pickle
import cv2
import os
import argparse
from pathlib import Path


def create_encodings(dataset, encodings_file):

    print('PWr W4 Projekt zespolowy 2020 \n M.Pietras, J.Grzana \n Program tworzacy plik z kodowaniem twarzy \n Na podstawie programow P.Ryana')

    # sciezki do zdjec znajdujacych sie w  bazie danych
    imagePaths = list(paths.list_images(dataset))

    # inicjowanie zmiennych
    knownEncodings = []
    knownNames = []

    # petla po wszytskich obrazach
    for (i, imagePath) in enumerate(imagePaths):
        
        name = imagePath.split(os.path.sep)[-2]

        # wczytanie i konwersja obrazu
        image = cv2.imread(imagePath)
        rgb_image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

	# odnajdywanie twarzy - zapisywanie wszpolrzednych boksow
        boxes = face_recognition.face_locations(rgb_image, model='hog')

	# kodowanie twarzy - zapisywanie 128 liczb
        encodings = face_recognition.face_encodings(rgb_image, boxes)

        for encoding in encodings:
            knownEncodings.append(encoding)
            knownNames.append(name)

    data = {"encodings": knownEncodings, "names": knownNames}

    # uzupelnianie juz istniejacego zbioru o nowe dane
    if os.path.exists(encodings_file):
        with open(encodings_file, mode="rb") as opened_file:
            results = pickle.load(opened_file,encoding='latin1')
            data['encodings'].extend(results['encodings'])
            data['names'].extend(results['names'])

    # zapisanie kodowania do pliku 
    f = open(encodings_file, "wb")
    f.write(pickle.dumps(data))
    f.close()


if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    ap.add_argument("-d", "--dataset", required=True, help="sciezka do bazy danych.")
    ap.add_argument("-e", "--encodings-file", required=True, help="sciezka do pliku z kodowaniem twarzy.  Jezeli plik juz istnieje, mozna go uzupelnic o nowe dane.")

    args = vars(ap.parse_args())

    create_encodings(args['dataset'], args['encodings_file'])


