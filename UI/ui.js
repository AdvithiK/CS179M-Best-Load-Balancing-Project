const canvas = document.getElementById("ShipBox");
const ctx = canvas.getContext("2d");

const rows = 8;      
const cols = 12;     
const colWidth = canvas.width/cols;
const rowLength = canvas.height/rows;
let firstEnter = true;


function drawGrid() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.strokeStyle = "black";

    for (let c = 0; c <= cols; c++) {
      ctx.beginPath();
      ctx.moveTo(c * colWidth, 0);
      ctx.lineTo(c * colWidth, canvas.height);
      ctx.stroke();
    }

    for (let r = 0; r <= 12; r++) {
        ctx.beginPath();
        ctx.moveTo(0, r*rowLength);
        ctx.lineTo(canvas.width, r*rowLength);
        ctx.stroke();
    }
}

function drawEmptyContainers() {
    ctx.fillStyle = "grey";
    ctx.strokeStyle = "black";

    containers.forEach(con => {
      let px = con.x * colWidth;    
      let py = (rows - 1 - con.y) * rowLength;  // y starts from the bottom

      ctx.fillRect(px, py, colWidth, rowLength);
      ctx.strokeRect(px, py, colWidth, rowLength);

      ctx.fillStyle = "black";
      ctx.fillStyle = "grey";
    });

}

function drawContainers() {
  const colors = [
    "red",
    "blue",
    "green",
    "yellow",
    "orange",
    "purple",
    "pink",
    "cyan",
    "magenta",
    "lime",
    "teal",
    "brown",
    "gold",
    "navy",
    "olive",
    "coral",
  ];


    fetch("data.json")
      .then(response => response.json())
      .then(containers => {
          let colorIndex = 0;
          containers.forEach(con => {
          let px = con.x * colWidth;    
          let py = (rows - 1 - con.y) * rowLength;  // y starts from the bottom

          // *** some kind of check if the function is NaN or not ***
          ctx.fillStyle = colors[colorIndex];
          colorIndex++;

          ctx.fillRect(px, py, colWidth, rowLength);
          ctx.strokeRect(px, py, colWidth, rowLength);

          ctx.fillStyle = "black";           
          ctx.font = "27px Arial";           
          ctx.fillText(con.name, px + 5, py+25); 
        });

      });
}



function showManifestScreen() {
    document.getElementById("enter-manifest-screen").style.display = "block";
    document.getElementById("initial-screen").style.display = "none";
    document.getElementById("ready-screen").style.display = "none";
    document.getElementById("next-move-screen").style.display = "none";
}


function showInitialScreen() {
    document.getElementById("enter-manifest-screen").style.display = "none";
    document.getElementById("initial-screen").style.display = "block";
    document.getElementById("ready-screen").style.display = "none";
    document.getElementById("next-move-screen").style.display = "none";
    drawGrid();
}

function showReadyScreen() {
    document.getElementById("enter-manifest-screen").style.display = "none";
    document.getElementById("initial-screen").style.display = "none";
    document.getElementById("ready-screen").style.display = "block";
    document.getElementById("next-move-screen").style.display = "none";
    drawGrid();
}

function nextMoveScreen() {
  document.getElementById("enter-manifest-screen").style.display = "none";
  document.getElementById("initial-screen").style.display = "none";
  document.getElementById("ready-screen").style.display = "none";
  document.getElementById("next-move-screen").style.display = "block";
  drawGrid();
  drawContainers();
}


async function updatetoReady() {
    try {
        const res = await fetch("data.json?t=" + Date.now()); // check for updated json file
        const data = await res.json();

        // change this to showNextNodeScreen()
        // showReadyScreen();
        

    } catch (err) {
        console.log("Still loading...");
        showInitialScreen();
    }
}

async function updateData() {
    try {
        const res = await fetch("data.json?t=" + Date.now()); // check for updated json file
        const data = await res.json();

        showReadyScreen();
        

    } catch (err) {
        console.log("Still loading...");
        showInitialScreen();
    }
}

// document.addEventListener("keydown", (e) => {
//     if (e.key === "Enter") {
//       if (!firstEnter) {
//           handleManifest(); 
//           updateData();
//           updatetoReady();
//           firstEnter = true;
//         } else {
//             nextMoveScreen();
//         }
//     }
  
// });

document.addEventListener("DOMContentLoaded", () => {
    // Submit button
    const btn = document.getElementById("submit-btn");

    function handleManifest() {
      const value = document.getElementById("text-box").value;
      console.log("User typed:", value);

      fetch("http://localhost:5051/run", {
          method: "POST",
          body: value
      })
      .then(res => res.text())
      .then(text => console.log("Server responded:", text))
      .catch(err => console.error("Fetch error:", err));
    }

    btn.addEventListener("click", handleManifest);

    // Enter key handling
    document.addEventListener("keydown", (e) => {
        if (e.key === "Enter") {
            if (!firstEnter) {
                handleManifest();
                firstEnter = true;
            } else {
                nextMoveScreen();
            }
        }
    });

    showManifestScreen();
});





