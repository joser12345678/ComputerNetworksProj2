from asyncio import threads
import random
import subprocess
import os
import hashlib

def test_func(url):
    threads = str(random.randrange(1, 11))
    print('Testing: ' + url + ' Thread number: ' + threads)
    a = subprocess.run(['./http_downloader', '-u', url, '-o', 'test_out', '-n', threads])
    assert a.returncode == 0

    b = subprocess.run(['wget', '--user-agent="josephtest"', '-O', 'test_true', url, '--no-check-certificate'])
    assert b.returncode == 0

    sha = hashlib.sha1()
    with open('test_true', 'rb') as f1:
        chunk = 0
        while chunk != b'':
            chunk = f1.read(1024)
            sha.update(chunk)

    sha2 = hashlib.sha1()
    with open('test_out', 'rb') as f2:
        chunk = 0
        while chunk != b'':
            chunk = f2.read(1024)
            sha2.update(chunk)

    assert sha.hexdigest() == sha2.hexdigest()
    a = subprocess.run(['rm', 'test_out', 'test_true'])


test_func('https://cobweb.cs.uga.edu/~perdisci/CSCI6760-F21/Project2-TestFiles/topnav-sport2_r1_c1.gif')
test_func('https://cobweb.cs.uga.edu/~perdisci/CSCI6760-F21/Project2-TestFiles/Uga-VII.jpg')
test_func('https://cobweb.cs.uga.edu/~perdisci/CSCI6760-F21/Project2-TestFiles/story_hairydawg_UgaVII.jpg')
test_func('https://www.softwright.com/wp-content/themes/softwright-2021/assets/images/softwright-logo.png')
test_func('https://arxiv.org/pdf/2104.01725.pdf')
test_func('https://arxiv.org/pdf/1402.0484.pdf')
test_func('https://seedsecuritylabs.org/Labs_20.04/Files/Buffer_Overflow_Setuid/Buffer_Overflow_Setuid.pdf')
test_func('https://previews.123rf.com/images/houbacze/houbacze1809/houbacze180900156/110390100-banner-examples.jpg?fj=1')
test_func('https://ece.vt.edu/content/dam/ece_vt_edu/people/134A4002.jpg')
print('SUCCESS BBY')