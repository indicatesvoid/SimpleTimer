## SimpleTimer - A timer library for Arduino.
##### Modified to support member function callbacks

### But Why?
The original SimpleTimer library is great. But what if you want to clean up your Arduino sketch and pull out its components into individual classes? What then?

This modification of SimpleTimer (I call it AgnosticTimer, but renaming it to SimpleTimer here for parity with the original, since this is a fork) utilizes C template classes to support member function callbacks.

### Basic Usage
You can create a new timer like so:
```
SimpleTimer<MySpecialClass> timer;
```
Where 'MySpecialClass' is the name of the class the timer will be calling to for its callbacks.

Then, simply call setInterval to set up the interval and callback for the timer:
```
timer.setInterval(3000, this, &MySpecialClass::timerElapsed);
```
The first argument is the number of milliseconds for the timer, the second is a reference to the class that holds the function you wish to call (always 'this', if you're calling it from within the class), and the third argument is a reference to the member function that will be called each time the timer elapses.

Then, make sure to call the run method from within your loop() function, again passing in a reference to the class that holds your callback:
```
timer.run(this);
```

### Original Uploader
Derek Chafin  
Jun 16, 2012  
info<a href="http://www.google.com/recaptcha/mailhide/d?k=01OlINdKCVggEULcwNY4NTTg==&amp;c=NuRwkCvdkiHnX97lENoQ4aUrQPIek0qZKoNXiOiv8uE=" onclick="window.open('http://www.google.com/recaptcha/mailhide/d?k\07501OlINdKCVggEULcwNY4NTTg\75\75\46c\75NuRwkCvdkiHnX97lENoQ4aUrQPIek0qZKoNXiOiv8uE\075', '', 'toolbar=0,scrollbars=0,location=0,statusbar=0,menubar=0,resizable=0,width=500,height=300'); return false;" title="Reveal this e-mail address">...</a>@gmail.com

### Modified by
William Clark
https://github.com/indicatesvoid

###Disclaimer
For reference this library can also be found at http://playground.arduino.cc/Code/SimpleTimer.

### Original Author
Marcello Romani  
mrom<a href="http://www.google.com/recaptcha/mailhide/d?k=01OlINdKCVggEULcwNY4NTTg==&amp;c=35t9v0kxxmeXywbgEkq2W9ogg7J-gbHsx2HD-cJD7Rc=" onclick="window.open('http://www.google.com/recaptcha/mailhide/d?k\07501OlINdKCVggEULcwNY4NTTg\75\75\46c\07535t9v0kxxmeXywbgEkq2W9ogg7J-gbHsx2HD-cJD7Rc\075', '', 'toolbar=0,scrollbars=0,location=0,statusbar=0,menubar=0,resizable=0,width=500,height=300'); return false;" title="Reveal this e-mail address">...</a>@ottotecnica.com  
Copyright (c) 2010 OTTOTECNICA Italy

### License - GNU LGPL 2.1+
This library is free software; you can redistribute it
and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software
Foundation; either version 2.1 of the License, or (at
your option) any later version.

This library is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser
General Public License along with this library; if not,
write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA