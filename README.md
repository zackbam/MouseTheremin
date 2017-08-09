# MouseTheremin
Play Theremin with a mouse pointer. Appropriate for head-mouses as well
In the configuration file the following parameters are set:

firstMidiNote 60 : determines the lowest note in midi value. 60 is for do.

semitones 25 : determines the range.

windowHeight 0.25 : The portion the window takes. A value of 1 would cover the whole screen.

slopeXStability 70 : The highest angle of mouse direction in degrees at which the x displasment is ignored. This is implemented in order to maintain the pitch unchanged when changing the volume. A value of 70, means that when the mouse moves verticaly 70-90 degrees, the x displacement is ignored.

vibratoWidth 0.45 : The vibrato width is semitones.

vibratoFrequency 5.5 : the vibrato frequency in Hz.
