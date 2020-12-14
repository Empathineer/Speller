<h1 id="speller"><a data-id="" href="#speller">Speller</a></h1>

<main class="col-md markdown-body" style="margin-bottom: 25px;">


<p>Implement a program that spell-checks a file, a la the below, using a hash table.</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>$ ./speller texts/lalaland.txt
MISSPELLED WORDS

[...]
AHHHHHHHHHHHHHHHHHHHHHHHHHHHT
[...]
Shangri
[...]
fianc
[...]
Sebastian's
[...]

WORDS MISSPELLED:
WORDS IN DICTIONARY:
WORDS IN TEXT:
TIME IN load:
TIME IN check:
TIME IN size:
TIME IN unload:
TIME IN TOTAL:
</code></pre></div></div>


<h3 id="understanding"><a data-id="" href="#understanding">Understanding</a></h3>

<p>Theoretically, on input of size <em>n</em>, an algorithm with a running time of <em>n</em> is “asymptotically equivalent,” in terms of <em>O</em>, to an algorithm with a running time of <em>2n</em>. Indeed, when describing the running time of an algorithm, we typically focus on the dominant (i.e., most impactful) term (i.e., <em>n</em> in this case, since <em>n</em> could be much larger than 2). In the real world, though, the fact of the matter is that <em>2n</em> feels twice as slow as <em>n</em>.</p>

<p>The challenge ahead of you is to implement the fastest spell checker you can! By “fastest,” though, we’re talking actual “wall-clock,” not asymptotic, time.</p>

<p>In <code class="highlighter-rouge">speller.c</code>, we’ve put together a program that’s designed to spell-check a file after loading a dictionary of words from disk into memory. That dictionary, meanwhile, is implemented in a file called <code class="highlighter-rouge">dictionary.c</code>. (It could just be implemented in <code class="highlighter-rouge">speller.c</code>, but as programs get more complex, it’s often convenient to break them into multiple files.) The prototypes for the functions therein, meanwhile, are defined not in <code class="highlighter-rouge">dictionary.c</code> itself but in <code class="highlighter-rouge">dictionary.h</code> instead. That way, both <code class="highlighter-rouge">speller.c</code> and <code class="highlighter-rouge">dictionary.c</code> can <code class="highlighter-rouge">#include</code> the file. Unfortunately, we didn’t quite get around to implementing the loading part. Or the checking part. Both (and a bit more) we leave to you! But first, a tour.</p>

<h4 id="dictionaryh"><a data-id="" href="#dictionaryh"><code class="highlighter-rouge">dictionary.h</code></a></h4>

<p>Open up <code class="highlighter-rouge">dictionary.h</code>, and you’ll see some new syntax, including a few lines that mention <code class="highlighter-rouge">DICTIONARY_H</code>. No need to worry about those, but, if curious, those lines just ensure that, even though <code class="highlighter-rouge">dictionary.c</code> and <code class="highlighter-rouge">speller.c</code> (which you’ll see in a moment) <code class="highlighter-rouge">#include</code> this file, <code class="highlighter-rouge">clang</code> will only compile it once.</p>

<p>Next notice how we <code class="highlighter-rouge">#include</code> a file called <code class="highlighter-rouge">stdbool.h</code>. That’s the file in which <code class="highlighter-rouge">bool</code> itself is defined. You’ve not needed it before, since the CS50 Library used to <code class="highlighter-rouge">#include</code> that for you.</p>

<p>Also notice our use of <code class="highlighter-rouge">#define</code>, a “preprocessor directive” that defines a “constant” called <code class="highlighter-rouge">LENGTH</code> that has a value of <code class="highlighter-rouge">45</code>. It’s a constant in the sense that you can’t (accidentally) change it in your own code. In fact, <code class="highlighter-rouge">clang</code> will replace any mentions of <code class="highlighter-rouge">LENGTH</code> in your own code with, literally, <code class="highlighter-rouge">45</code>. In other words, it’s not a variable, just a find-and-replace trick.</p>

<p>Finally, notice the prototypes for five functions: <code class="highlighter-rouge">check</code>, <code class="highlighter-rouge">hash</code>, <code class="highlighter-rouge">load</code>, <code class="highlighter-rouge">size</code>, and <code class="highlighter-rouge">unload</code>. Notice how three of those take a pointer as an argument, per the <code class="highlighter-rouge">*</code>:</p>

<div class="language-c highlighter-rouge"><div class="highlight"><pre class="highlight"><code><span class="n">bool</span> <span class="nf">check</span><span class="p">(</span><span class="k">const</span> <span class="kt">char</span> <span class="o">*</span><span class="n">word</span><span class="p">);</span>
<span class="kt">unsigned</span> <span class="kt">int</span> <span class="nf">hash</span><span class="p">(</span><span class="k">const</span> <span class="kt">char</span> <span class="o">*</span><span class="n">word</span><span class="p">);</span>
<span class="n">bool</span> <span class="nf">load</span><span class="p">(</span><span class="k">const</span> <span class="kt">char</span> <span class="o">*</span><span class="n">dictionary</span><span class="p">);</span>
</code></pre></div></div>

<p>Recall that <code class="highlighter-rouge">char *</code> is what we used to call <code class="highlighter-rouge">string</code>. So those three prototypes are essentially just:</p>

<div class="language-c highlighter-rouge"><div class="highlight"><pre class="highlight"><code><span class="n">bool</span> <span class="nf">check</span><span class="p">(</span><span class="k">const</span> <span class="n">string</span> <span class="n">word</span><span class="p">);</span>
<span class="kt">unsigned</span> <span class="kt">int</span> <span class="nf">hash</span><span class="p">(</span><span class="k">const</span> <span class="n">string</span> <span class="n">word</span><span class="p">);</span>
<span class="n">bool</span> <span class="nf">load</span><span class="p">(</span><span class="k">const</span> <span class="n">string</span> <span class="n">dictionary</span><span class="p">);</span>
</code></pre></div></div>

<p>And <code class="highlighter-rouge">const</code>, meanwhile, just says that those strings, when passed in as arguments, must remain constant; you won’t be able to change them, accidentally or otherwise!</p>

<h4 id="dictionaryc"><a data-id="" href="#dictionaryc"><code class="highlighter-rouge">dictionary.c</code></a></h4>

<p>Now open up <code class="highlighter-rouge">dictionary.c</code>. Notice how, atop the file, we’ve defined a <code class="highlighter-rouge">struct</code> called <code class="highlighter-rouge">node</code> that represents a node in a hash table. And we’ve declared a global pointer array, <code class="highlighter-rouge">table</code>, which will (soon) represent the hash table you will use to keep track of words in the dictionary. The array contains <code class="highlighter-rouge">N</code> node pointers, and we’ve set <code class="highlighter-rouge">N</code> equal to <code class="highlighter-rouge">1</code> for now, meaning this hash table has just 1 bucket right now. You’ll likely want to increase the number of buckets, as by changing <code class="highlighter-rouge">N</code>, to something larger!</p>

<p>Next, notice that we’ve implemented <code class="highlighter-rouge">load</code>, <code class="highlighter-rouge">hash</code>, <code class="highlighter-rouge">check</code>, <code class="highlighter-rouge">size</code>, and <code class="highlighter-rouge">unload</code>, but only barely, just enough for the code to compile. Your job, ultimately, is to re-implement those functions as cleverly as possible so that this spell checker works as advertised. And fast!</p>

<h4 id="spellerc"><a data-id="" href="#spellerc"><code class="highlighter-rouge">speller.c</code></a></h4>

<p>Okay, next open up <code class="highlighter-rouge">speller.c</code> and spend some time looking over the code and comments therein. You won’t need to change anything in this file, and you don’t need to understand its entirety, but do try to get a sense of its functionality nonetheless. Notice how, by way of a function called <code class="highlighter-rouge">getrusage</code>, we’ll be “benchmarking” (i.e., timing the execution of) your implementations of <code class="highlighter-rouge">check</code>, <code class="highlighter-rouge">load</code>, <code class="highlighter-rouge">size</code>, and <code class="highlighter-rouge">unload</code>. Also notice how we go about passing <code class="highlighter-rouge">check</code>, word by word, the contents of some file to be spell-checked. Ultimately, we report each misspelling in that file along with a bunch of statistics.</p>

<p>Notice, incidentally, that we have defined the usage of <code class="highlighter-rouge">speller</code> to be</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>Usage: speller [dictionary] text
</code></pre></div></div>

<p>where <code class="highlighter-rouge">dictionary</code> is assumed to be a file containing a list of lowercase words, one per line, and <code class="highlighter-rouge">text</code> is a file to be spell-checked. As the brackets suggest, provision of <code class="highlighter-rouge">dictionary</code> is optional; if this argument is omitted, <code class="highlighter-rouge">speller</code> will use <code class="highlighter-rouge">dictionaries/large</code> by default. In other words, running</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>$ ./speller text
</code></pre></div></div>

<p>will be equivalent to running</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>$ ./speller dictionaries/large text
</code></pre></div></div>

<p>where <code class="highlighter-rouge">text</code> is the file you wish to spell-check. Suffice it to say, the former is easier to type! (Of course, <code class="highlighter-rouge">speller</code> will not be able to load any dictionaries until you implement <code class="highlighter-rouge">load</code> in <code class="highlighter-rouge">dictionary.c</code>! Until then, you’ll see <code class="highlighter-rouge">Could not load</code>.)</p>

<p>Within the default dictionary, mind you, are 143,091 words, all of which must be loaded into memory! In fact, take a peek at that file to get a sense of its structure and size. Notice that every word in that file appears in lowercase (even, for simplicity, proper nouns and acronyms). From top to bottom, the file is sorted lexicographically, with only one word per line (each of which ends with <code class="highlighter-rouge">\n</code>). No word is longer than 45 characters, and no word appears more than once. During development, you may find it helpful to provide <code class="highlighter-rouge">speller</code> with a <code class="highlighter-rouge">dictionary</code> of your own that contains far fewer words, lest you struggle to debug an otherwise enormous structure in memory. In <code class="highlighter-rouge">dictionaries/small</code> is one such dictionary. To use it, execute</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>$ ./speller dictionaries/small text
</code></pre></div></div>

<p>where <code class="highlighter-rouge">text</code> is the file you wish to spell-check. Don’t move on until you’re sure you understand how <code class="highlighter-rouge">speller</code> itself works!</p>

<p>Odds are, you didn’t spend enough time looking over <code class="highlighter-rouge">speller.c</code>. Go back one square and walk yourself through it again!</p>

<h4 id="texts"><a data-id="" href="#texts"><code class="highlighter-rouge">texts/</code></a></h4>

<p>So that you can test your implementation of <code class="highlighter-rouge">speller</code>, we’ve also provided you with a whole bunch of texts, among them the script from <em>La La Land</em>, the text of the Affordable Care Act, three million bytes from Tolstoy, some excerpts from <em>The Federalist Papers</em> and Shakespeare, the entirety of the King James V Bible and the Koran, and more. So that you know what to expect, open and skim each of those files, all of which are in a directory called <code class="highlighter-rouge">texts</code> within your <code class="highlighter-rouge">pset5</code> directory.</p>

<p>Now, as you should know from having read over <code class="highlighter-rouge">speller.c</code> carefully, the output of <code class="highlighter-rouge">speller</code>, if executed with, say,</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>$ ./speller texts/lalaland.txt
</code></pre></div></div>

<p>will eventually resemble the below. For now, try the staff’s solution (using the default dictionary) by executing</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>$ ~cs50/2019/fall/pset5/speller texts/lalaland.txt
</code></pre></div></div>

<p>Below’s some of the output you’ll see. For information’s sake, we’ve excerpted some examples of “misspellings.” And lest we spoil the fun, we’ve omitted our own statistics for now.</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code>MISSPELLED WORDS

[...]
AHHHHHHHHHHHHHHHHHHHHHHHHHHHT
[...]
Shangri
[...]
fianc
[...]
Sebastian's
[...]

WORDS MISSPELLED:
WORDS IN DICTIONARY:
WORDS IN TEXT:
TIME IN load:
TIME IN check:
TIME IN size:
TIME IN unload:
TIME IN TOTAL:
</code></pre></div></div>

<p><code class="highlighter-rouge">TIME IN load</code> represents the number of seconds that <code class="highlighter-rouge">speller</code> spends executing your implementation of <code class="highlighter-rouge">load</code>. <code class="highlighter-rouge">TIME IN check</code> represents the number of seconds that <code class="highlighter-rouge">speller</code> spends, in total, executing your implementation of <code class="highlighter-rouge">check</code>. <code class="highlighter-rouge">TIME IN size</code> represents the number of seconds that <code class="highlighter-rouge">speller</code> spends executing your implementation of <code class="highlighter-rouge">size</code>. <code class="highlighter-rouge">TIME IN unload</code> represents the number of seconds that <code class="highlighter-rouge">speller</code> spends executing your implementation of <code class="highlighter-rouge">unload</code>. <code class="highlighter-rouge">TIME IN TOTAL</code> is the sum of those four measurements.</p>

<p><strong>Note that these times may vary somewhat across executions of <code class="highlighter-rouge">speller</code>, depending on what else CS50 IDE is doing, even if you don’t change your code.</strong></p>

<p>Incidentally, to be clear, by “misspelled” we simply mean that some word is not in the <code class="highlighter-rouge">dictionary</code> provided.</p>


<h2 id="specification"><a data-id="" href="#specification">Objectives & Specification</a></h2>

<p>Our objective was to implement the functions <code class="highlighter-rouge">load</code>, <code class="highlighter-rouge">hash</code>, <code class="highlighter-rouge">size</code>, <code class="highlighter-rouge">check</code>, and <code class="highlighter-rouge">unload</code> as efficiently as possible using a hash table in such a way that <code class="highlighter-rouge">TIME IN load</code>, <code class="highlighter-rouge">TIME IN check</code>, <code class="highlighter-rouge">TIME IN size</code>, and <code class="highlighter-rouge">TIME IN unload</code> are all minimized. Reducing space is a desired goal, but the ultimate one was performance time optimization. Here are the program requirements:</p>

<ul class="fa-ul">
  <li data-marker="*"><span class="fa-li"><i class="fas fa-circle"></i></span>You may not alter <code class="highlighter-rouge">speller.c</code> or <code class="highlighter-rouge">Makefile</code>.</li>
  <li data-marker="*"><span class="fa-li"><i class="fas fa-circle"></i></span>You may alter <code class="highlighter-rouge">dictionary.c</code> (and, in fact, must in order to complete the implementations of <code class="highlighter-rouge">load</code>, <code class="highlighter-rouge">hash</code>, <code class="highlighter-rouge">size</code>, <code class="highlighter-rouge">check</code>, and <code class="highlighter-rouge">unload</code>), but you may not alter the declarations (i.e., prototypes) of <code class="highlighter-rouge">load</code>, <code class="highlighter-rouge">hash</code>, <code class="highlighter-rouge">size</code>, <code class="highlighter-rouge">check</code>, or <code class="highlighter-rouge">unload</code>. You may, though, add new functions and (local or global) variables to <code class="highlighter-rouge">dictionary.c</code>.</li>
  <li data-marker="*"><span class="fa-li"><i class="fas fa-circle"></i></span>You may change the value of <code class="highlighter-rouge">N</code> in <code class="highlighter-rouge">dictionary.c</code>, so that your hash table can have more buckets.</li>
  <li data-marker="*"><span class="fa-li"><i class="fas fa-circle"></i></span>You may alter <code class="highlighter-rouge">dictionary.h</code>, but you may not alter the declarations of <code class="highlighter-rouge">load</code>, <code class="highlighter-rouge">hash</code>, <code class="highlighter-rouge">size</code>, <code class="highlighter-rouge">check</code>, or <code class="highlighter-rouge">unload</code>.</li>
  <li data-marker="*"><span class="fa-li"><i class="fas fa-circle"></i></span>Your implementation of <code class="highlighter-rouge">check</code> must be case-insensitive. In other words, if <code class="highlighter-rouge">foo</code> is in dictionary, then <code class="highlighter-rouge">check</code> should return true given any capitalization thereof; none of <code class="highlighter-rouge">foo</code>, <code class="highlighter-rouge">foO</code>, <code class="highlighter-rouge">fOo</code>, <code class="highlighter-rouge">fOO</code>, <code class="highlighter-rouge">fOO</code>, <code class="highlighter-rouge">Foo</code>, <code class="highlighter-rouge">FoO</code>, <code class="highlighter-rouge">FOo</code>, and <code class="highlighter-rouge">FOO</code> should be considered misspelled.</li>
  <li data-marker="*"><span class="fa-li"><i class="fas fa-circle"></i></span>Capitalization aside, your implementation of <code class="highlighter-rouge">check</code> should only return <code class="highlighter-rouge">true</code> for words actually in <code class="highlighter-rouge">dictionary</code>. Beware hard-coding common words (e.g., <code class="highlighter-rouge">the</code>), lest we pass your implementation a <code class="highlighter-rouge">dictionary</code> without those same words. Moreover, the only possessives allowed are those actually in <code class="highlighter-rouge">dictionary</code>. In other words, even if <code class="highlighter-rouge">foo</code> is in <code class="highlighter-rouge">dictionary</code>, <code class="highlighter-rouge">check</code> should return <code class="highlighter-rouge">false</code> given <code class="highlighter-rouge">foo's</code> if <code class="highlighter-rouge">foo's</code> is not also in <code class="highlighter-rouge">dictionary</code>.</li>
  <li data-marker="*"><span class="fa-li"><i class="fas fa-circle"></i></span>You may assume that any <code class="highlighter-rouge">dictionary</code> passed to your program will be structured exactly like ours, alphabetically sorted from top to bottom with one word per line, each of which ends with <code class="highlighter-rouge">\n</code>. You may also assume that <code class="highlighter-rouge">dictionary</code> will contain at least one word, that no word will be longer than <code class="highlighter-rouge">LENGTH</code> (a constant defined in <code class="highlighter-rouge">dictionary.h</code>) characters, that no word will appear more than once, that each word will contain only lowercase alphabetical characters and possibly apostrophes, and that no word will start with an apostrophe.</li>
  <li data-marker="*"><span class="fa-li"><i class="fas fa-circle"></i></span>You may assume that <code class="highlighter-rouge">check</code> will only be passed words that contain (uppercase or lowercase) alphabetical characters and possibly apostrophes.</li>
  <li data-marker="*"><span class="fa-li"><i class="fas fa-circle"></i></span>Your spell checker may only take <code class="highlighter-rouge">text</code> and, optionally, <code class="highlighter-rouge">dictionary</code> as input. Although you might be inclined (particularly if among those more comfortable) to “pre-process” our default dictionary in order to derive an “ideal hash function” for it, you may not save the output of any such pre-processing to disk in order to load it back into memory on subsequent runs of your spell checker in order to gain an advantage.</li>
  <li data-marker="*"><span class="fa-li"><i class="fas fa-circle"></i></span>Your spell checker must not leak any memory. Be sure to check for leaks with <code class="highlighter-rouge">valgrind</code>.</li>
  <li data-marker="*"><span class="fa-li"><i class="fas fa-circle"></i></span>You may search for (good) hash functions online, so long as you cite the origin of any hash function you integrate into your own code.</li>
</ul>


