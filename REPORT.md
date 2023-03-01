# Samantha Tang CMSC701 Project 1 Report 
My code for this project can be found at: [https://github.com/stang10/CMSC701_HW1](https://github.com/stang10/CMSC701_HW1).
## Part (a), constructing the suffix array and an auxiliary index
**What did you find to be the most challenging part of implementing the buildsa program?**

While serializing my data was a difficult part of this assignment, I realized that was simply a matter of refreshing myself on the topic. In truth, the most challenging part of implementing the buildsa program was constructing the prefix tables for a file. Under the guidance of questions on Piazza, I chose to use a dense representation of the prefix table. Therefore, I found the most difficult part being how to calculate which prefix would align to which index in the table. It took a period of experimentation with different values of k and various prefixes of length k to determine the mathematical formula for computing the index of a given prefix. In truth, its implementation in the code did not take much, but I felt it was the component of Part (a) that required me to think the most about what needed to be implemented rather than how to implement it.

**For references of various size, I answer the following questions in the table below:**

**How long does building the suffix array take?**

**How large is the resulting serialized file?**

| Reference | Size of Genome (in characters) | Size of Serialized File | Time to Build Suffix Array | Time to Run Program | Max RSS Size (Approx RAM) |
| ----------- | ----------- | ----------- | ----------- | ----------- | ----------- |
| NZ_KV839643.1 | 45625 | 411 KB | 3568594 nanoseconds | 0.01 seconds | 1252 KB |
| U00096.2 | 4639675 | 41.8 MB | 309803821 nanoseconds | 11.06 seconds | 165880 KB |
| CP100583.1 | 6729861 | 60.6 MB | 378330634 nanoseconds | 25.14 seconds | 231868 KB |

**Now I will try the --preftab option with some different values of k. The information will be stored in the tables below.**

For NZ_KV839643.1:

| k | Size of Serialized File | Time to Run Program | Max RSS Size (Approx RAM) |
| ----------- | ----------- | ----------- | ----------- |
| 2 | 411 KB | 0.01 seconds | 1256 KB | 
| 4 | 415 KB | 0.02 seconds | 1256 KB | 
| 6 | 476 KB | 0.02 seconds | 1428 KB | 

For U00096.2:

| k | Size of Serialized File | Time to Run Program | Max RSS Size (Approx RAM) |
| ----------- | ----------- | ----------- | ----------- |
| 2 | 41.757 MB | 11.86 seconds | 148588 KB | 
| 4 | 41.761 MB | 11.91 seconds | 154836 KB | 
| 6 | 41.822 MB | 12.02 seconds | 155424 KB | 

For CP100583.1:

| k | Size of Serialized File | Time to Run Program | Max RSS Size (Approx RAM) |
| ----------- | ----------- | ----------- | ----------- |
| 2 | 60.569 MB | 26.78 seconds | 239396 KB | 
| 4 | 60.572 MB | 27.45 seconds | 245628 KB | 
| 6 | 60.634 MB | 26.89 seconds | 250840 KB | 

**Given the scaling above, how large of a genome do you think you could construct the suffix array for on a machine with 32GB of RAM, why?**
It has been stated on multiple webpages that Max RSS is roughly the amount of RAM used by a program. As such, we can look to the values of Max RSS sizes for each of the sequence sizes to determine an approximate relationship between the sequence size and Max RSS size of the program using the sequence. 

Note: 4639675/45625 = 101.6915 and 165880/1252 = 132.4920.
Additionally: 6729861/4639675 = 1.4505 and 231868/165880 = 1.3978.

Though there are a small number of data points to draw a conclusion from, I would approximate that the Max RSS size scales proportionate to the sequence size. The values do not perfectly reflect this, but it aproximately trends this way. I will use this observation to make a prediction about the size of the genome that a suffix array could be constructed for with 32 GB of RAM. 

32 GB = 32000000 KB. 32000000/231868 = 138.0096. 6729861*138.0096 = 928785424.666 which rounds down to 928785424.

Therefore, I would approximate that you could construct a suffix array of a genome of size 928,785,424 with 32 GB of RAM. 

## Part (b), querying the suffix array

**What did you find to be the most challenging part of implementing the query program?**

The most challenging part of implementing the query program was determining the bounds in the suffix array for suffixes whose prefixes match a query string. That is to say, writing a binary search function to locate one hit in the suffix array that contained the query as a prefix was easy, but then determining the first and last indices of values in the suffix array that contains the query as a prefix was more difficult. Initially, my first thought was to simply iterate through indices lower than the first hit until the query is no longer a prefix and similarly for indices greater than the first hit. However, I realized for long genomes, depending on the length of the query string, we could be iterating for tens of thousands of indices in each direction. So, since we used binary search to find the first hit because of its improved search speed, I determined I should use it to search for the leftmost index where the query is a prefix in the suffix array and the rightmost index. I did this by searching within the bounds in which the first hit was found. (In other words if binary search with left index l and right index ended because we found the value at the midpoint value c between l and r, I then run binary search between l and c to determine if there are more matches to the left of c. I continue this process of searching between a hit and the left bound it was found within until -1 is returned meaning there are no more to the left of the hit. This is also mirrored for values to the right of c.)

**For references of various size, I answer the following questions in the table below:**

**How long does query take on references of different size, and on queries of different length?**

**How does the speed of the naive lookup algorithm compare to the speed of the simpleaccel lookup algorithm?**
For smaller sized references and smaller sized queries, we can see that it many cases the Simple Accelerant version does not perform better than the Naive version. However, as both the size of the reference and the size of queries increase, the Simple Accelerant does start to out perform the Naive version. 

| Reference | Size of Genome (in characters) | Size of Query (in characters) | Naive or SimpAccel | Time for Query | 
| ----------- | ----------- | ----------- | ----------- | ----------- | 
| NZ_KV839643.1 | 45625 | 8 (intended to fail) | Naive | 510262 nanoseconds | 
| NZ_KV839643.1 | 45625 | 6 | Naive | 47063 nanoseconds | 
| NZ_KV839643.1 | 45625 | 70 | Naive | 47589 nanoseconds | 
| NZ_KV839643.1 | 45625 | 700 | Naive | 64529 nanoseconds | 
| NZ_KV839643.1 | 45625 | 8 (intended to fail) | SimpAccel | 531280 nanoseconds | 
| NZ_KV839643.1 | 45625 | 6 | SimpAccel | 61880 nanoseconds | 
| NZ_KV839643.1 | 45625 | 70 | SimpAccel | 73666 nanoseconds | 
| NZ_KV839643.1 | 45625 | 700 | SimpAccel | 142185 nanoseconds | 
| U00096.2 | 4639675 | 8 (intended to fail) | Naive | 78720109 nanoseconds |
| U00096.2 | 4639675 | 6 | Naive | 56241818 nanoseconds |
| U00096.2 | 4639675 | 70 | Naive | 73918527 nanoseconds |
| U00096.2 | 4639675 | 700 | Naive | 79707024 nanoseconds |
| U00096.2 | 4639675 | 8 (intended to fail) | SimpAccel | 93960131 nanoseconds |
| U00096.2 | 4639675 | 6 | SimpAccel | 94557859 nanoseconds |
| U00096.2 | 4639675 | 70 | SimpAccel | 70811414 nanoseconds |
| U00096.2 | 4639675 | 700 | SimpAccel | 63340721 nanoseconds |
| CP100583.1 | 6729861 | 8 (intended to fail) | Naive | 114731312 nanoseconds |
| CP100583.1 | 6729861 | 6 | Naive | 112834037 nanoseconds |
| CP100583.1 | 6729861 | 70 | Naive | 90553634 nanoseconds |
| CP100583.1 | 6729861 | 700 | Naive | 115454868 nanoseconds |
| CP100583.1 | 6729861 | 8 (intended to fail) | SimpAccel | 119256990 nanoseconds |
| CP100583.1 | 6729861 | 6 | SimpAccel | 136418259 nanoseconds |
| CP100583.1 | 6729861 | 70 | SimpAccel | 96500861 nanoseconds |
| CP100583.1 | 6729861 | 700 | SimpAccel | 95613990 nanoseconds |

**How does the speed further compare when not using a prefix lookup table versus using a prefix lookup table with different values of k?**
In general, as k increases, the time to compute the query decreases however there are various queries that do not have their times to compute decrease. The time stays relatively the same or even becomes worse. 

| Reference | Size of Genome (in characters) | Size of Query (in characters) | Naive or SimpAccel | Time for Query Without Prefixes | Time for Query (k = 2) |  Time for Query (k = 4) | Time for Query (k = 6) | 
| ----------- | ----------- | ----------- | ----------- | ----------- |  ----------- | ----------- | ----------- | 
| NZ_KV839643.1 | 45625 | 8 (intended to fail) | Naive | 510262 nanoseconds | 380497 nanoseconds | 167842 nanoseconds | 1085 nanoseconds |
| NZ_KV839643.1 | 45625 | 6 | Naive | 47063 nanoseconds |  50592 nanoseconds |26598 nanoseconds| 50323 nanoseconds |
| NZ_KV839643.1 | 45625 | 70 | Naive | 47589 nanoseconds | 102537 nanoseconds |115093 nanoseconds| 72426 nanoseconds |
| NZ_KV839643.1 | 45625 | 700 | Naive | 64529 nanoseconds | 42913 nanoseconds |23923 nanoseconds | 14469 nanoseconds|
| NZ_KV839643.1 | 45625 | 8 (intended to fail) | SimpAccel | 531280 nanoseconds | 345909 nanoseconds | 181793 nanoseconds | 1198 nanoseconds |
| NZ_KV839643.1 | 45625 | 6 | SimpAccel | 61880 nanoseconds | 87665 nanoseconds | 89005 nanoseconds| 98105 nanoseconds |
| NZ_KV839643.1 | 45625 | 70 | SimpAccel | 73666 nanoseconds | 119247 nanoseconds| 136026 nanoseconds| 97750 nanoseconds|
| NZ_KV839643.1 | 45625 | 700 | SimpAccel | 142185 nanoseconds | 115252 nanoseconds | 90753 nanoseconds|74620 nanoseconds|
| U00096.2 | 4639675 | 8 (intended to fail) | Naive | 78720109 nanoseconds |  65940866 nanoseconds | 49907398 nanoseconds|32841360 nanoseconds|
| U00096.2 | 4639675 | 6 | Naive | 56241818 nanoseconds | 56325023 nanoseconds| 42094205 nanoseconds|26894985 nanoseconds|
| U00096.2 | 4639675 | 70 | Naive | 73918527 nanoseconds |48840864 nanoseconds| 48500844 nanoseconds|25537683 nanoseconds|
| U00096.2 | 4639675 | 700 | Naive | 79707024 nanoseconds |39708815 nanoseconds|38814981 nanoseconds |28487711 nanoseconds|
| U00096.2 | 4639675 | 8 (intended to fail) | SimpAccel | 93960131 nanoseconds | 74961523 nanoseconds| 55390046 nanoseconds|39639621 nanoseconds|
| U00096.2 | 4639675 | 6 | SimpAccel | 94557859 nanoseconds |84823265 nanoseconds|53203290 nanoseconds|34312689 nanoseconds|
| U00096.2 | 4639675 | 70 | SimpAccel | 70811414 nanoseconds |65057405 nanoseconds|55852154 nanoseconds| 54194297 nanoseconds|
| U00096.2 | 4639675 | 700 | SimpAccel | 63340721 nanoseconds |56397030 nanoseconds|  46502571 nanoseconds| 36716761 nanoseconds|
| CP100583.1 | 6729861 | 8 (intended to fail) | Naive | 114731312 nanoseconds | 100693185 nanoseconds| 100249434 nanoseconds|94996881 nanoseconds|
| CP100583.1 | 6729861 | 6 | Naive | 112834037 nanoseconds |98213206 nanoseconds|89963296 nanoseconds|42300303 nanoseconds|
| CP100583.1 | 6729861 | 70 | Naive | 90553634 nanoseconds |82155854 nanoseconds |61023072 nanoseconds|62461419 nanoseconds|
| CP100583.1 | 6729861 | 700 | Naive | 115454868 nanoseconds |83212478 nanoseconds|89038764 nanoseconds|58042999 nanoseconds|
| CP100583.1 | 6729861 | 8 (intended to fail) | SimpAccel | 119256990 nanoseconds |105572117 nanoseconds|109582218 nanoseconds|99105375 nanoseconds|
| CP100583.1 | 6729861 | 6 | SimpAccel | 136418259 nanoseconds |102039371 nanoseconds|93759084 nanoseconds|60319667 nanoseconds|
| CP100583.1 | 6729861 | 70 | SimpAccel | 96500861 nanoseconds |93015365 nanoseconds|73489007 nanoseconds|70177447 nanoseconds|
| CP100583.1 | 6729861 | 700 | SimpAccel | 95613990 nanoseconds |97551136 nanoseconds|85088400 nanoseconds|72069311 nanoseconds|

**Given the scaling above, and the memory requirements of each type of index, what kind of tradeoff do you personally think makes sense in terms of using more memory in exchange for faster search.**

As I stated above, greater values of k does appear to lead to a faster search in many cases. However, at a certain points, increased values of k make little to no improvement in the speed of the search. Additionally note, that while the k values increasing improves the speed up until a certain point due to the benefits of pre-computation from the prefix table, we have exponential growth in the size of the prefix table. As such, it does make sense to trade the memory used by the prefix tables for the resulting faster search up until a point, as at this point there will be diminishing return as k will offer little to no improvement but we will use exponentially more memory. 