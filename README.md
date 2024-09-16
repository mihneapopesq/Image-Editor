# Image-Editor

The task involves applying multiple operations on grayscale or RGB images. The operations include:

- Dynamically loading matrices into memory (LOAD file)
- Saving files in memory, either in binary or ASCII format (SAVE file/SAVE file ascii)
- Selecting regions from the image and storing their coordinates (SELECT x1, y1, x2, y2)
- Cropping an image (CROP)
- Applying filters to grayscale or RGB images (APPLY EDGE/SHARPEN/BLUR/GAUSSIAN_BLUR)
- Displaying the histogram of the image with a maximum number of y bins and x stars (HISTOGRAM x y)
- Equalizing images (EQUALIZE)
- Rotating images by angles divisible by 90 (ROTATE angle)
- Exiting the program and deallocating resources (EXIT)

## LOAD
In the load operation, the program checks the type of the loaded image.
P2 is a grayscale image in ASCII format, P3 is an RGB image in ASCII format, P5
is a grayscale image in binary format, and P6 is an RGB image in binary format.
I dynamically allocated the required matrix while storing the dimensions in `a->row`
and `a->column`, as well as the selected coordinates. If a matrix was loaded previously,
it is freed from memory. If the specified file does not exist, the message "Failed to load *filename*" will be displayed.

## SAVE
In the save operation, the processed image is saved to the specified file, either in
ASCII or binary format. Depending on the command, the word "ascii" may or may not follow after SAVE and the file name. If no matrix has been loaded, the message "No image loaded" will be displayed.

## SELECT
`SELECT x1 y1 x2 y2`
We receive coordinates, validate them, and load them into the image structure.
If the command `SELECT ALL` is used, `x1` and `y1` are set to 0, and `x2` and `y2` take the values of the number of columns and rows, thus selecting the entire image.

## CROP
Based on the current selection, the image is reduced to the portion within the selected
coordinates, in-place. The previous matrix is deallocated, and a new one with the corresponding dimensions is allocated.

## EQUALIZE
The equalization operation can only be applied to grayscale matrices, making use of the image’s histogram.

## HISTOGRAM
We receive `x` and `y`, the maximum number of stars, and the number of bins.
The histogram is displayed using '*'. The formula for the number of stars after each line is:  
`nr_stars = frequency_value / max_frequency * x`

## APPLY: EDGE, BLUR, SHARPEN, GAUSSIAN_BLUR
We receive a specific kernel for each filter, which is a 3x3 matrix with
preset elements. Based on the received filter, it is applied either to a portion or the entire image. I decided to store the kernels in matrices with descriptive names (edge, sharpen, blur, gaussian). Using the functions `clamp_min` and `clamp_max`,
I can set each element that exceeds the range [0, 255] to 0 or 255.

## ROTATE +-{0, 90, 180, 270, 360}
I decided to implement the operation using a single 90-degree right rotation,
since I can apply it twice for a 180 or -180 degree rotation, or three times for a 270 or -90 degree rotation. If the selection is not square or the entire image is not selected, the ROTATE operation cannot be executed.

## EXIT:
The exit function deallocates all used resources.

## Implementation
In the main function, I use function calls, aided by parsing the characters
following them. In this way, I can check if the commands are invalid, or if the parameters
of some commands are incorrectly called.

# Image Structure (struct.h)

In `row` and `column`, the dimensions of the image for rows and columns are stored. In `max`, the maximum value of a channel is stored. `x1`, `x2`, `y1`, and `y2` represent the selected coordinates.
`type` stores the image type (RGB or binary), containing P2, P3, P5, or P6. `load` will have 0 if no image is loaded, or 1 if an image is already loaded. `message` will be used in cases where we display "Easy, Charlie Chaplin" so that "APPLY parameter invalid" is not also displayed, since the command is correct. The matrices `mat`, `tmp`, `tmpr`, and `RGB` store the grayscale matrix, an auxiliary matrix used for some grayscale operations, another auxiliary matrix but of RGB type, and the RGB matrix.
