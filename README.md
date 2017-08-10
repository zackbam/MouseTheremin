# MouseTheremin
Play Theremin with a mouse pointer. Appropriate for head-mouses as well
In the configuration file the following parameters are set:

firstMidiNote 60 : determines the lowest note in midi value. 60 is for do.

semitones 25 : determines the range.

windowHeight 0.25 : The portion the window takes. A value of 1 would cover the whole screen.

slopeXStability 70 : The highest angle of mouse direction in degrees at which the x displasment is ignored. This is implemented in order to maintain the pitch unchanged when changing the volume. A value of 70, means that when the mouse moves verticaly 70-90 degrees, the x displacement is ignored.

vibratoWidth 0.45 : The vibrato width in semitones.

vibratoFrequency 5.5 : the vibrato frequency in Hz.

harmonics 500 300 80 5 2 2 2 

the max value of a harmonic is 1000, but to avoid distortion, the total sum of all should not exceed 1000. For a warm sound have low values in the high harmonics, or simply do not set them at all. For a more aggressive, use the high harmonics.
For example, with the values above, when you play a note at 100Hz the sound will be a weighted sum of the 7 first harmonics:
0.5*sin(100) + 0.3*sin(200) + 0.08 * sin(300) + 0.005* sin(400) + 0.002 * sin(500) + 0.002 * sin(600) + 0.002 * sin(700)
