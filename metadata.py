import configparser, os

SECTION = "SETTINGS"
CONFIG_FILE = 'config.cfg'
INVALID_CHARS = '\/:*?"<>|'

config = configparser.ConfigParser()
try:
    config.read_file(open(CONFIG_FILE))
    author = config.get(SECTION, 'author')
    title = config.get(SECTION, 'title')
    
except:
    author = input('Author: ')
    title = input('Title: ')
    if any (x in title for x in INVALID_CHARS):
        raise Exception("Bad character in title")
    config.add_section(SECTION)
    config.set(SECTION, 'author', author)
    config.set(SECTION, 'title', title)
    config.write(open(CONFIG_FILE,'w'))


print(author, title)


if __name__ == '__main__':
    import webbrowser
    url = 'https://www.google.com.tr/search?q="{}+{}+audiobook'.format(artist.replace(' ','+'), title.replace(' ','+'))    
    webbrowser.open(url)