//#include "ProdTutorial/ProducerTest/plugins/frameStriding.h"
#include "E2eDL/E2eDLrec/plugins/frameStriding.h"

// Striding input frames (vDetFrame by rowstrides and colstrides accordingly)
std::vector<std::vector<float>> frameStriding(std::vector<float>& vDetFrame, int rows, int columns, int rowstrides, int colstrides){
  std::vector<std::vector<float>> vStridedFrame ((rows*rowstrides), std::vector<float> (columns*colstrides,0));
  for (int rowidx=0; rowidx<rows; rowidx++){
    for (int colidx=0; colidx<columns; colidx++){
      for (int kernelrow=0; kernelrow<rowstrides; kernelrow++){
        for (int kernelcol=0; kernelcol<colstrides; kernelcol++){
          vStridedFrame[rowstrides*rowidx+kernelrow][colstrides*colidx+kernelcol] = vDetFrame[rowidx*columns+colidx]/(rowstrides*colstrides);
          //if(rowidx<5 && colidx<5) std::cout<<"("<<rowstrides*rowidx+kernelrow<<","<<colstrides*colidx+kernelcol<<"): "<<vStridedFrame[rowstrides*rowidx+kernelrow][colstrides*colidx+kernelcol]<<" "<<vDetFrame[rowidx*columns+colidx]/(rowstrides*colstrides);
        }
      }
    }
  }
  return vStridedFrame;
}
