Cantina Tech Talk - October 2014
===

Why create Objectively?
---

> C++ is a horrible language. It's made more horrible by the fact that a lot 
> of substandard programmers use it, to the point where it's much much 
> easier to generate total and utter crap with it. Quite frankly, even if 
> the choice of C were to do *nothing* but keep the C++ programmers out, 
> that in itself would be a huge reason to use C. -- Linus Torvalds

 * C++ is unruly and not always welcome in some open source projects.  
 * C99 is widely supported and offers massive improvements to C89.
 * Opportunity to grow my understanding of C and possibly other languages.

First attempt
---

    Interface(Foo, Object)
        int bar;
        void (*baz)(Foo *);
    End
    
 * Heavy use of macros (preprocessor abuse)
 * Instance methods live on each instance
 * _Archetype_ instance maintained for each type
  * Highly flexible and very simple, but inefficient
  * Lots of copying on instantiation
  * Memory consumed by _archetype_ instances
 * While maybe novel, this forfeits the hallmark of C.

Second attempt (current implementation)
---
 * Sparse use of macros, use code templates for convenience
 * Instance and interface types are separate
 * Instances include a pointer to their typed interface
  * Allows for faster instantiation
  * Complicates polymorphism
 * This is how most OO languages work

Lessons learned
---
 * I take reflection for granted
  * No storage class for `typeof` limits usefulness
 * OO compilers maintain additional scope within compilation units
 * `strncpy` does not like overlapping strings, but `memcpy` doesn't care
 
What's next?
---
 * More robust collections framework
 * Networking framework
 * MVC framework using libSDL2
 * Take advantage of C11 (e.g. anonymous structs, native threads, ..)
 
***

Quake
===
![Penumbral Wake - Quake2World](https://fbcdn-sphotos-f-a.akamaihd.net/hphotos-ak-xpf1/t31.0-8/861008_617224188323107_1551552667_o.jpg)

In a nut shell
---
 * Binary space partitioning splits volumes on planes
 * Volumes are split until they are convex
 
 ![Binary space partitioning](http://upload.wikimedia.org/wikipedia/commons/8/81/Binary_space_partition.png)
 
 * Planes can be represented as a normal vector and distance


    typedef float vec_t;
    typedef vec_t[2] vec2_t;
    typedef vec_t[3] vec3_t;
    typedef vec_t[4] vec4_t;
    typedef vec4_t plane_t;


 * Volumes can therefore be defined by a set of clipping planes
 * _Dot-product_ can yield plane sidedness


    vec_t dot(plane_t p, vec3_t v) {
    	return (p[0] * v[0] + p[1] * v[1] + p[2] * v2[2]) - p[3];
    }

 * This is the basis for *all* collision detection and visibility checking in Quake
 * [Further reading on BSP format](https://developer.valvesoftware.com/wiki/Source_BSP_File_Format)

Architecture
---
 * Server
  * Game API
   * Game module
    * Physics, ballistics, gameplay rules
  * Protocol
 * Client
  * Client game API
   * Client game module
    * Scene management
    * Physics (prediction)
  * Input
  * Renderer
  * Sound
  * Protocol
  
Network protocol
---
 * Implemented over UDP with ad-hoc reliable delivery
  * Allows for in-band and out-of-band packets
 * Packets are sequenced to detect drops and enable delta compression
 * Delta compression requires a circular buffer of previous states at both ends
 
![Circular buffer](http://www.mathcs.emory.edu/~cheung/Courses/171/Syllabus/8-List/FIGS/queue08b.gif)
 
 * Client applies linear interpolation between network updates to render a smooth simulation
  * The client is usually rendering slightly behind the last received server state
 * Client side prediction runs player movement commands locally to anticipate server state at frame `N + 1`

Game / client game API
---
 * The basis of Quake's success and longevity
 * Separate license for game SDK
 * Allows alternate gameplay modes to be quickly implemented by 3rd parties
 * _Capture the Flag_, _Team Fortress_, _CounterStrike_