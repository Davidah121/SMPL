For a linear gradient of just 2 points (A start and stop point with corresponding colors), the need for a gather function is non-existent.
A simple approach for any number of x values in a horizontal line (all we care about) is to collect the start and end point percentage values and then find the lerp value for each x.
    This is a simple thing to do and it does not matter what the actual lerp blend value is since it will work out provided no division by 0.
    SSE operations is just:
        blendValue = subtraction, subtraction, division
        SimpleGraphics::lerp for the color which is already implemented

When throwing in multiple key points with different colors, gather and scatter becomes important

Due to the requirement to gather required key points and colors, just brute force it.
Brute force
    For each 4/8/whatever, project onto the line.
    For each value gotten, gather the required values (stop key points and colors)
    Lerp between them

sudo code?:

sseY = SSE value for input y - toPoint.y
sseToPX = SSE values for toPoint.x
normX = SSE values for normalized toPoint.x
normY = SSE values for normalized toPoint.y
len = SSE values for length

    for every 4 x values:
        projectedValues = (x-sseToPX) * normX + sseY * normY / len

        gather 4 (total 8) keyPoints, and 4 (total 8) colors as arrays

        startKeyPoint = load(gatheredStartKeyPoints)
        endKeyPoint = load(gatheredEndKeyPoints)
        divValues = (endKeyPoint - startKeyPoint)
        divValues = replaceZeroWithOne(divValues)
        lerpValue = (projectedValues - startKeyPoint) / divValues
        outputColor = lerp(startColors, endColors, lerpValues)
        blend(outputColor, imageColor)

    for remaining:
        drawColor(x, y, surf) //lazy

