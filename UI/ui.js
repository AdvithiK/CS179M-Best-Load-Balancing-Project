const canvas = document.getElementById("ShipBox");
const ctx = canvas.getContext("2d");

const rows = 9; 
const visiblerows = 8;     
const cols = 12;     
const colWidth = canvas.width/cols;
const internalrow_length = canvas.height/visiblerows;
const visiblerow_length = canvas.height/visiblerows
let firstEnter = true;
let finalScreenShowing = false;
let skipToEnd = false;

function drawCrane() {
  const x = 0 * colWidth;   
  const y = 9 * internalrow_length;  

  ctx.fillStyle = "red";
  ctx.globalAlpha = 0.7;
  ctx.fillRect(x, y, colWidth, internalrow_length);
  ctx.globalAlpha = 1.0;
}

function drawGrid() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.strokeStyle = "black";

    for (let c = 0; c <= cols; c++) {
      ctx.beginPath();
      ctx.moveTo(c * colWidth, 0);
      ctx.lineTo(c * colWidth, canvas.height);
      ctx.stroke();
    }

    for (let r = 0; r <= visiblerows; r++) {
        ctx.beginPath();
        ctx.moveTo(0, r*visiblerow_length);
        ctx.lineTo(canvas.width, r*visiblerow_length);
        ctx.stroke();
    }
}


function drawContainers() {
  ctx.clearRect;
  const colors = [
  "#FFD700", 
  "#FFA500", 
  "#FFB6C1", 
  "#87CEFA", 
  "#90EE90", 
  "#FF69B4", 
  "#FFE4B5", 
  "#98FB98", 
  "#AFEEEE", 
  "#F0E68C", 
  "#D8BFD8", 
  "#E0FFFF", 
  "#FFDAB9", 
  "#FFEFD5", 
  "#F5DEB3",
  "#F08080"  
];


    fetch("data.json?ts="+ Date.now())
      .then(response => response.json())
      .then(containers => {
          let colorIndex = 0;
          containers.forEach(con => {
          
          let px = con.x * colWidth;
          let py = (con.y-1) * visiblerow_length;

          
          // skip if its the crane
          if (py != 9) {
            // if name is NAN, color the box grey
            if (con.name == "NAN") ctx.fillStyle = "grey";
            else {
              ctx.fillStyle = colors[colorIndex];
              colorIndex++;
            }

            ctx.fillRect(px, py, colWidth, visiblerow_length);
            ctx.strokeRect(px, py, colWidth, visiblerow_length);

            ctx.fillStyle = "black";           
            ctx.font = "12px Arial";           
            ctx.fillText(con.name, px + 1, py+25); 
          }
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
    drawContainers();
}

function nextMoveScreen() {
  document.getElementById("enter-manifest-screen").style.display = "none";
  document.getElementById("initial-screen").style.display = "none";
  document.getElementById("ready-screen").style.display = "none";
  document.getElementById("next-move-screen").style.display = "block";
  drawGrid();
  drawContainers();
}

function showFinalScreen() {
  document.getElementById("final-screen").style.display = "block";
  document.getElementById("enter-manifest-screen").style.display = "none";
  document.getElementById("initial-screen").style.display = "none";
  document.getElementById("ready-screen").style.display = "none";
  document.getElementById("next-move-screen").style.display = "none";
  drawGrid();
  drawContainers();
  finalScreenShowing = true;
}


document.addEventListener("DOMContentLoaded", () => {

    const btn = document.getElementById("submit-btn");
    const log_btn = document.getElementById("log-btn");

  

    function handleComment() {
      const comment = document.getElementById("comment-box").value;
      console.log(comment);

      fetch("http://localhost:8080/input", {
          method: "POST",
          headers: { "Content-Type": "text/plain" },
          body: comment
      })
      .then(res => res.text())
      .then(text => { 
        console.log(text)
        document.getElementById("comment-box").value = "";
      })
      .catch(err => console.error("Fetch error:", err));
      
    }

    function handleManifest() {
      const value = document.getElementById("text-box").value;
      console.log("User typed:", value);

      fetch("http://localhost:8080/run", {
          method: "POST",
          headers: { "Content-Type": "text/plain" },
          body: value
      })
      .then(res => res.text())
      .then(text => { 
        console.log(text)
        if(text != "") {
          const parts = text.split(",");
          const moves = parts[0];
          const minutes = parts[1];

          document.querySelector("#ready-screen h1:nth-child(2)").textContent = `${moves} move(s)`;
          document.querySelector("#ready-screen h1:nth-child(3)").textContent = `${minutes} minute(s)`;
          let enter_message = "Hit ENTER when ready for first move";
          if (parts[0] != 0) { 
            document.querySelector("#ready-screen").append(enter_message);
            skipToEnd = true;
          }
          showReadyScreen(); 

        } else {
            alert("Algorithm failed or file not found!");
        }
      })
      .catch(err => console.error("Fetch error:", err));
      
    }

    btn.addEventListener("click", handleManifest);
    log_btn.addEventListener("click", handleComment);

    
    document.addEventListener("keydown", (e) => {
        if (e.key === "Enter" && !finalScreenShowing) {
            fetch("http://localhost:8080/nextmove", { method: "POST" })
              .then(res => res.text())       
              .then(text => {
                const finalScreen = document.getElementById("final-screen");
                const nextScreen = document.getElementById("next-move-screen");
                const cleanText = text.trim();

                if (cleanText.includes("OUTBOUND")) {
                   
                    nextScreen.textContent = "";
                    finalScreen.textContent = "I have written an updated manifest to the desktop as "+ cleanText +". Don't forget to email it to the captain. Hit ENTER when done";
              
                    showFinalScreen(); 
                } else {
  
                    const p = document.createElement("p");
                    p.textContent = cleanText;
                    nextScreen.appendChild(p);
                    nextMoveScreen();
                }
              })
              .catch(err => console.error("Next move fetch error:", err));
        } else if (e.key === "Enter") {
          fetch("http://localhost:8080/exit", { method: "POST" })
            .then(res => res.text())
            .then(text => console.log("Server response:", text))
            .catch(err => console.error("Exit fetch error:", err));
        }
    });

});





