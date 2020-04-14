from face_recognition.api import face_distance
import argparse
import pickle
import cv2
import face_recognition

def get_encoding_for_image(imagePath):

    print('PWr W4 Projekt zespolowy 2020 \n M.Pietras, J.Grzana \n Program obliczajacy podobienstwo twarzy \n 0.0 - calkowite podobienstwo \n 100.0 - brak podobienstwa \n Na podstawie programow P.Ryana')
 
    # wczytanie obrazu/portretu pamieciowego
    image = cv2.imread(imagePath)
    rgb_image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

    # odnajdywanie twarzy - zapisywanie wszpolrzednych boksow
    boxes = face_recognition.face_locations(rgb_image, model='hog')

    # kodowanie twarzy - zapisywanie 128 liczb
    encodings = face_recognition.face_encodings(rgb_image, boxes)

    # zalozenie, ze na tym zdjeciu znajduje sie tylko jedna twarz
    return encodings[0]

if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    ap.add_argument("-i", "--new-image", required=True,
                    help="sciezka do obrazu, z ktorym porownywane beda zdjecia z bazy dancyh")
    ap.add_argument("-e", "--encodings-file", required=True,
                    help="sciezka do pliku z kodowaniem twarzy")
    args = vars(ap.parse_args())

    new_encoded_image = get_encoding_for_image(args['new_image'])

    # odczytywanie kodowania twarzy z bazy danych
    results = pickle.loads(open(args['encodings_file'], "rb").read())

    encodings = results['encodings']
    names = results['names']
    
    # obliczanie odleglosci - czyli podobienstwa twarzy i wypisywanie ich
    distance_results = face_distance(encodings, new_encoded_image)
    results = list(zip(distance_results, names))
    sorted_list = sorted(results, key=lambda x: x[0])
    for x in sorted_list:
        print(x)

