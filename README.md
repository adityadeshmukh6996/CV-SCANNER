# CV-SCANNER
A common application of image processing or computer vision is a document scanner.
They are typically used on mobiledevices. Maybe you have used one already. 
we want a solution which does detect the document automatically and then provides a proper scan of the document. 

Goals :
For our scanner to work, we need to fulfil a three step algorithm:
• Calculate the position of the four corner points
• Use a perspective transformation to get a clean top-view of the document.
• Filter the scanned document to get a clear binary image.
