# Fibonacci Clock

The Fibonacci Clock has been designed for curious and inventive people who like a time piece that keeps them on their toes. The clock uses the famous Fibonacci sequence to display time in a brand new way. 

# How do I tell time on the Fibonacci Clock?

The Fibonacci sequence is a sequence of numbers created by the Italian mathematician Fibonacci in the 13th century. This is a sequence starting with 1 and 1, where each subsequent number is the sum of the previous two. For the clock I used the first 5 terms: 1, 1, 2, 3 and 5.   

The screen of the clock is made up of five squares whose side lengths match the first five Fibonacci numbers: 1, 1, 2, 3 and 5. The hours are displayed using red and the minutes using green. When a square is used to display both the hours and minutes it turns blue. White squares are ignored. 

To tell time on the Fibonacci clock you need to do some math. To read the hour, simply add up the corresponding values of the red and blue squares. To read the minutes, do the same with the green and blue squares. The minutes are displayed in 5 minute increments (0 to 12) so you have to multiply your result by 5 to get the actual number. 

Often, there are multiple ways to display a single time. To add to the challenge, the combinations are picked randomly from all the different ways a number can be displayed. There are, for example, 16 different ways to display 6:30 and you never know which one the clock will use!

# Hack it, it's open!

The Fibonacci clock is driven by an Atmega328 micro-controller running Arduino. That means you can change the code running in the clock using the official Arduino IDE. The possibilities are as infinite as the Fibonacci sequence! I can't wait to see what you will come up with.

To make it easy to hack, I have included an FTDI connector so that you don't have to remove the chip to reprogram it. On top of that, a yellow LED has been connected to pin #13 to help you debug your application.

The Fibonacci Clock is an open source / hardware project. Once all the rewards are shipped I'll publish all the sources and the schematics on my website under the GPL license.
