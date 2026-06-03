In an ideal scenario, all flatter shaded can easily be converted into a pure vector graphic. Ignoring the background aspects of an illustration, which doesn't matter in this case because game assets are often separated, as they make the process more difficult.

All art can be broken into segments that can be 3 types of shading.
- Flat shading
- Gradient shading
- Randomized shading

Sharp changes might seem like it should get its own shading but actually, those would be its own segment entirely. This results in far more segments than you'd think.
    You might think that the hair of a character should be one part or maybe 2 or 3 for a front and back part so you have proper depth layering and easier animation.
    In practice, it'll be an extra part for any sharp enough change that could be percieved as an edge.

Randomized shading are segments that contain colors or color gradients that don't follow an obvious pattern. These sadly have no solution that results in a good vector graphic.

Gradient shading are all other segments. They are either a radial gradient or linear gradient and both may contain multiple key points adjusting how the gradients work.

The main idea is to convert into edges, use the flood-fill algo idea to determine sections or trace boundaries, convert each section to a vector path with its own color / fill method if a gradient is used.

The tricky part comes from approximating the gradient over a patch. This is mathematically weird. Assuming a consistent gradient, you could solve the direction and intensity.

If the gradient is simple and linear (not radial), the direction is the average of all directions and the intensity is the average intensity. 
This isn't true for non simple non linear gradients. For those cases, you need the peaks / dips to split the gradient into multiple parts where you can apply the simple method. 
    Need a good zero finding method that can find a variable number of zeroes.

This is the main issue. Approximating a gradient function over a gradient field