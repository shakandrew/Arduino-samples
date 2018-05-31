# RD02 motor software
## How to run next code?
Connect RD02 and LCD05 to Arduino. If you don't know how, look [here](http://www.robot-electronics.co.uk/htm/Lcd05tech.htm) and [here](http://www.robot-electronics.co.uk/htm/md25tech.htm). How to run it?
First of all you need to work a little with python. You need to install `pip3`
and `virtualenv`. After installing them create new virtual environment:

`python3 -m venv /path/to/new/virtual/environment`

 More details, how to use it [here](https://virtualenv.pypa.io/en/stable/userguide/).

 Activate it. Then run:

 `pip install -r requirements.txt`

 And to run application:

 `python communicate.py <bit rate> <route to arduino's port>`

 Have fun ;)
