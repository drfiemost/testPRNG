# A PRNG comparison [WiP]

*A quick comparison of simple and fast psudo-random number generators*

Been looking for a fast and possibly unbiased pseudo-random number generator for a video game, so I decided to check out a few alternatives. The generated numbers must be in a range of [0,N) with N relatively small, usually not exceeding 10.000, so 16 bit are more than sufficient. Speed is favored over quality as long as the output looks random enough to a casual observer. Having the numbers distributed evenly throughout the range would be a nice addition but we can live with a slightly biased output.

The algorithms considered are the following:

* Lemire: a fast 16 bit random number generator by Daniel Lemire [1];
* xorshift16: a 16 bit version of the Xorshift generator [2];
* LCG: a linear congruential generator [3] with MSVC parameters;
* standard: the standard C rand() function.


## Output

As a first check we just dump a thousands of numbers produced by the generators and plot then on a graph to see if they look random. Weird thing is that the Lemire output doesn't seem random at all but shows a pattern repeating every ~50 data. The others look quite good, sufficiently resembling white noise.

![Output of the Lemire generator](images/Lemire_dump.png)
![Output of the xorshift16 generator](images/xorshift16_dump.png)
![Output of the LCG generator](images/LCG_dump.png)
![Output of the standard generator](images/standard_dump.png)


## Distribution

The second check is based on an answer on stackoverflow [4] and is aimed at measuring the bias of the distribution. We produce ten million numbers in the range [0,100) and count the occurences of each one. The Lemire generator uses rejection to eliminate the bias. The other methods use a simple modulo but for such small ranges there is no noticeable issue.

![Distribution of the Lemire generator](images/Lemire_distribution.png)
![Distribution of the xorshift16 generator](images/xorshift16_distribution.png)
![Distribution of the LCG generator](images/LCG_distribution.png)
![Distribution of the standard generator](images/standard_distribution.png)

Zooming in the xorgshift graph we can notice a jump around the value 35:

![Distribution of the xorshift16 generator](images/xorshift16_distribution_zoomed.png)


## Timings

While running the second test the program also measure the time used to generate the ten million numbers.

Over multiple runs the results are consistent, here's a sample:
```
Lemire: time elapsed: 0.0454349 seconds
xorshift16: time elapsed: 0.0659807 seconds
LCG: time elapsed: 0.0535795 seconds
standard: time elapsed: 0.332941 seconds
```

The test program has been compiled with gcc version 10.3.1 using the "-O3" flag and run on an AMD FX(tm)-6300


## Conclusions

[_TODO_]

## References

[1] https://lemire.me/blog/2019/07/03/a-fast-16-bit-random-number-generator/  
[2] https://en.wikipedia.org/wiki/Xorshift  
[3] https://en.wikipedia.org/wiki/Linear_congruential_generator  
[4] https://stackoverflow.com/questions/11758809/what-is-the-optimal-algorithm-for-generating-an-unbiased-random-integer-within-a  
