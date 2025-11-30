const canvas = document.getElementById("ShipBox");
const ctx = canvas.getContext("2d");

const rows = 8;      
const cols = 12;     
const colWidth = canvas.width/cols
const rowLength = canvas.height/rows

// examples
const containers = [
    { name: "A1", x: 0, y: 0 },
    { name: "B3", x: 2, y: 1 },
    { name: "C7", x: 6, y: 3 }
];

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

function drawContainer() {
    ctx.fillStyle = "grey"; // edit this -> each box gets some ID mapped to the color ?
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


drawGrid();
drawEmptyContainers();