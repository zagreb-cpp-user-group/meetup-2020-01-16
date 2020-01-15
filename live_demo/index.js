class FaceDetectorApp {
    constructor( wasmModule ) {
        this.wasmModule = wasmModule;
        this.detectionResult = document.getElementById( 'detectionResult' );

        // create FaceDetector object (a c++ object)
        this.faceDetector = new this.wasmModule.FaceDetector();
    }

    adjustDetectionResults( videoFeed ) {
        this.detectionResult.width = videoFeed.videoWidth;
        this.detectionResult.height = videoFeed.videoHeight;
    }

    captureFrame( imageSource ) {
        const imageWidth = imageSource.videoWidth;
        const imageHeight = imageSource.videoHeight;

        // create working canvas (for image capture)
        this.canvas = this.canvas || document.createElement( 'canvas' );
        this.canvas.width = imageWidth;
        this.canvas.height = imageHeight;
        const ctx = this.canvas.getContext( '2d' );
        ctx.drawImage( imageSource, 0, 0, this.canvas.width, this.canvas.height );
        return ctx.getImageData( 0, 0, this.canvas.width, this.canvas.height );
    }

    detectFaces( imageSource ) {
        const imageData = this.captureFrame( imageSource );
        const detection = this.faceDetector.detectFaces( imageData );
        // draw the result, if successful
        if ( detection.detected ) {
            console.log( "Face detected at (" + detection.x + ", " + detection.y + ", " + detection.width + ", " + detection.height + ")" );
            const ctx = this.detectionResult.getContext( '2d' );
            ctx.clearRect( 0, 0, this.detectionResult.width, this.detectionResult.height );
            ctx.strokeStyle = '#FF0000FF';
            ctx.strokeRect( detection.x, detection.y, detection.width, detection.height );
        } else {
            console.log( "Nothing detected!" );
        }
        setTimeout( () => { this.detectFaces( imageSource ); }, 20 );
    }
}

// load WebAssembly module

FaceDetectorModule().then(
    ( module ) => {
        console.log( "WASM loaded successfully!" );

        window.faceDetector = new FaceDetectorApp( module );

        // open camera
        if ( navigator.mediaDevices && navigator.mediaDevices.getUserMedia ) {
            const constraints = {
                audio: false,
                video: {
                    width: { ideal: 1280 },
                    height: { ideal: 720 },
                    facingMode: { ideal: 'user' }
                }
            };
            navigator.mediaDevices.getUserMedia( constraints ).then(
                ( stream ) =>
                {
                    let cameraFeed = document.getElementById( 'cameraFeed' );
                    cameraFeed.controls = false;
                    if ( 'srcObject' in cameraFeed ) {
                        cameraFeed.srcObject = stream;
                    }
                    else {
                        cameraFeed.src = URL.createObjectURL( stream );
                    }
                    cameraFeed.play().then(
                        () => {
                            window.faceDetector.adjustDetectionResults( cameraFeed );
                            setTimeout( () => { window.faceDetector.detectFaces( cameraFeed ); }, 1 );
                        }
                    );
                }
            ).catch(
                error => {
                    console.log( error.name + ': ' + error.message ); //NotFoundError, NotAllowedError
                    alert( error.name + ': ' + error.message );
                }
            );
        }
        else {
            alert( "No support for media devices!" );
        }
    }
);

