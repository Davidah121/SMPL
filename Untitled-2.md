Notes:
- Dealing with drawing efficiently
    - Drawing efficiently can be broken into 2 parts.
        - Drawing really fast
        - Only drawing what is required
    - Ideally, just use a GPU but I'm complicated so that comes later
    - For drawing really fast:
        - Use a mixture of SIMD instructions and multiple threads.
            - SSE, AVX, AVX2, AVX512, OPENMP or a custom multithreaded approach.
                - Remember that threads aren't free even a thread pool as efficient as OMP
                can cause issues so use when the problem is deemed large enough otherwise its slower
    - For only drawing what is required:
        - *Efficient bounds calculations to skip unnecessary locations
            - Just needs to be faster than just doing the default naive approach.
            - Avoid polygons as even though they have tighter bounds, they add more complexity to the drawing code to deal with such bounds.
        - For a GUI, 2 areas are needed to determine what should be redrawn and what is okay to just avoid.
            - For each object being redrawn directly, its old bounding box (the area it originally resided in) consist of things that needs to be redrawn and its new bounding box (the area its moving too) consist of things that may need to be redrawn if it has transparency or its moving behind an object with transparency.
            - In the best case, transparency is not a factor and therefore you can skip redrawing anything in the new area that already was there. Just the old area.
            - Maintain 2 boxes that represent the 2 areas that will need to be adjusted.
            - If any of them have a non 0 width and height, those objects need in that section need to be re-rendered. This means 2 re-render passes at maximum.
            - If the boxes overlapp completely, it can and should be merged into 1 render pass.
            - Its best to keep them separate as you can avoid creating a much larger area that needs to be redrawn forcing objects and sections that are completely fine to be redrawn that could lead to far more objects being drawn than expected for no reason.

- Modifications to GUI
    - Gui needs a means to redraw an object twice without permanent marks until its done everything it needs.
    - This implies that render code for any gui object CAN NOT have side effects regarding the object itself. At the very least, 2 repeated calls must result in the same rendering code run with the same values.
        - This is why update is separated from rendering since it is guaranteed that update is always run and consistent.