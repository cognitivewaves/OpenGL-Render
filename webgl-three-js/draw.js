var renderer, camera, control, scene;

// =========== Pyramid Data ====================================================

var nCoordsComponents = 3;
var nColorComponents = 3;
var nLines = 3;
var nVerticesPerLine = 2;
var nFaces = 6;
var nVerticesPerFace = 3;


//  (3,4,5)          (6,7,8)
//     1----------------2
//     | \            / |
//     |   \        /   |
//     |     \    /     |
//     |        4       | (12,13,14)
//     |     /    \     |
//     |   /        \   |
//     | /            \ |
//     0 ---------------3
//  (0,1,2)          (9,10,11)

// pv Pyramid Vertices
var pv = [ 0.5, 0.5, 0.5,    // 0
           0.5, 1.5, 0.5,    // 1
           1.5, 1.5, 0.5,    // 2
           1.5, 0.5, 0.5,    // 3
           1.0, 1.0, 1.5 ];  // 4

// pvi Pyramid Vertex Indices (per face)
var pvi = [ 0, 1, 2,
            2, 3, 0,
            0, 3, 4,
            3, 2, 4,
            2, 1, 4,
            1, 0, 4 ];

// pei Pyramid Edge Indices
var pei = [ 0, 1, 
            1, 2, 
            2, 3, 
            3, 0,
            0, 4,
            1, 4,
            2, 4,
            3, 4 ];

// pve Pyramid Vertices Expanded
var pve = new Float32Array(nFaces*nVerticesPerFace*nCoordsComponents);
function expandPyramidVertices()
{
    for (i=0; i<nFaces; i++)
    {
        for (j=0; j<nVerticesPerFace; j++)
        {
            for (k=0; k<nCoordsComponents; k++)
            {
                pve[(i*3+j)*3+k] = pv[pvi[i*3+j]*3+k];
            }
        }
    }
}

// pc Pyramid colors
var pc = [ 0.3, 0.30, 0.3,
           1.0, 0.70, 0.0,
           1.0, 0.62, 0.0,
           1.0, 0.40, 0.0,
           1.0, 0.48, 0.0 ];

// pci Pyramid Color Indices (per face)
var pci = [ 0, 0, 0,
            0, 0, 0,
            1, 1, 1,
            2, 2, 2,
            3, 3, 3,
            4, 4, 4 ];

var pce = new Float32Array(nFaces*nVerticesPerFace*nColorComponents);
function expandPyramidColors()
{
    for (i=0; i<nFaces; i++)
    {
        for (j=0; j<nVerticesPerFace; j++)
        {
            for (k=0; k<nColorComponents; k++)
            {
                pce[(i*3+j)*3+k] = pc[pci[i*3+j]*3+k];
            }
        }
    }
}

// =============================================================================

function init()
{
    // Initialize renderer
    renderer = new THREE.WebGLRenderer( {antialias:true} );
    var width = window.innerWidth;
    var height = window.innerHeight;
    renderer.setSize (width, height);
    document.body.appendChild (renderer.domElement);

    // Setup camera
    camera = new THREE.PerspectiveCamera (45, width/height, 1, 10000);
    camera.position.x = 2;
    camera.position.y = -5;
    camera.position.z = 10;
    camera.lookAt (new THREE.Vector3(0,0,0));

    // Setup mouse orbit controls
    controls = new THREE.OrbitControls (camera, renderer.domElement);

    // Create an empty scene
    scene = new THREE.Scene();
}

// =============================================================================

function createModel()
{
    // Grid on the XZ plane
    var gridXZ = new THREE.GridHelper(5, 10, new THREE.Color(0xffffff), new THREE.Color(0x99999999));
    gridXZ.geometry.rotateX( Math.PI / 2 );
    scene.add(gridXZ);

    // Global X,Y,Z axes
    var axes = new THREE.AxesHelper( 5 );
    scene.add(axes);

    expandPyramidVertices();
    expandPyramidColors();

    // Pyramid faces
    var solidGeometry = new THREE.BufferGeometry();
    solidGeometry.addAttribute('position', new THREE.Float32BufferAttribute(pve, 3));
    solidGeometry.addAttribute('color', new THREE.Float32BufferAttribute(pce,3));
    var geomMaterial = new THREE.MeshBasicMaterial ({vertexColors: THREE.VertexColors});
    var geomMesh  = new THREE.Mesh (solidGeometry, geomMaterial);

    // Pyramid edges
    var lineGeometry = new THREE.BufferGeometry();
    lineGeometry.addAttribute('position', new THREE.Float32BufferAttribute(pv, 3));
    lineGeometry.setIndex(pei);
    var edgeMaterial = new THREE.LineBasicMaterial( { color: 0xffffff, transparent: true, opacity: 0.5 } );
    var lineSegments = new THREE.LineSegments(lineGeometry, edgeMaterial);
    
    model = new THREE.Group();
    model.add(geomMesh);
    model.add(lineSegments);
    
    scene.add(model);
}

// =============================================================================

function animate()
{
    controls.update();
    requestAnimationFrame ( animate );  
    renderer.render (scene, camera);
}

// =============================================================================

init();
createModel();
animate();

