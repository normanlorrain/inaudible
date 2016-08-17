artist = "Ken Follett"
title = "Eye of the Needle"


if __name__ == '__main__':
    import webbrowser
    url = 'https://www.google.com.tr/search?q="{}+{}+audiobook'.format(artist.replace(' ','+'), title.replace(' ','+'))    
    webbrowser.open(url)