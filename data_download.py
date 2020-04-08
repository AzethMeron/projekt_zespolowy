import sys
import argparse
import wget

print('PWr W4 Projekt zespolowy 2020 \n M.Pietras, J.Grzana \n Program do pobierania danych \n')

if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    ap.add_argument("-u", "--url", required=True,
                    help="adres url")
    ap.add_argument("-f", "--file", required=True,
                    help="sciezka zapisu pliku")
    args = vars(ap.parse_args())
    wget.download(args['url'], args['file'])