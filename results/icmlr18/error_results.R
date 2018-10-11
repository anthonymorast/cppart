run <- function(save_acc_plots=FALSE, save_imp_plots=FALSE) {

    results <- read.csv("./error_to_depth.csv", stringsAsFactors = FALSE)
    print(results)
    colnames(results)[6] <- "classification"
    results$classification <- as.numeric(results$classification)/100
    
    cars.delayed <- results[results$dataset == "cars" & results$delayed == 1,]
    cars.greedy <- results[results$dataset == "cars" & results$delayed == 0,]
    wine.greedy <- results[results$dataset == "wine" & results$delayed == 0,]
    wine.delayed <- results[results$dataset == "wine" & results$delayed == 1,]
    protein.delayed <- results[results$dataset == "protein" & results$delayed == 1,]
    protein.greedy <- results[results$dataset == "protein" & results$delayed == 0,]
    protein.greedy <- results[results$dataset == "sensit" & results$delayed == 0,]
    protein.greedy <- results[results$dataset == "protein" & results$delayed == 0,]
    sensit.greedy <- results[results$dataset == "sensit" & results$delayed == 0,]
    sensit.delayed <- results[results$dataset == "sensit" & results$delayed == 1,]
    c4.delayed <- results[results$dataset == "connect-4" & results$delayed == 1,]
    c4.greedy <- results[results$dataset == "connect-4" & results$delayed == 0,]
    mnist.delayed <- results[results$dataset == "mnist" & results$delayed == 1,]
    mnist.greedy <- results[results$dataset == "mnist" & results$delayed == 0,]
    
    if(save_acc_plots) {
        ############ Depth vs. Accuracy plots ############
        setEPS()
        postscript("depth_plots/depth_vs_accuracy4x2.eps")
        layout(matrix(c(1, 2, 3, 4, 5, 6, 7, 7), 4, 2, byrow=TRUE), heights=c(6, 6, 6, 3.8))
        #layout(matrix(c(1, 2, 3, 4, 5, 6), 3, 2, byrow=TRUE))#, heights=c(6, 6))
        par(xpd=NA)
        
        # Cars
        #setEPS()
        #postscript("depth_plots/cars_acc2dep.eps")
        plot(cars.greedy$mae~wine.delayed$depth, type="o", pch=17, col="red3", ylim=c(3.2, 4), xlab="Depth", ylab="MAE", main="Cars")
        lines(cars.delayed$mae~wine.delayed$depth, type="o", pch=19, col="blue3")
        
        # Wine
        #par(mai=rep(0.5, 4))
        #postscript("depth_plots/wine_acc2dep.eps")
        plot(wine.greedy$classification~wine.greedy$depth, type="o", pch=17, col="red3", ylim=c(.51, .6), xlab="Depth", ylab="Test Accuracy", main="Wine")
        lines(wine.delayed$classification~wine.delayed$depth, type="o", pch=19, col="blue3")
        #legend(5, .6, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        #dev.off()
        
        # SensIT
        #setEPS()
        #postscript("depth_plots/sensit_acc2dep.eps")
        plot(sensit.greedy$classification~sensit.greedy$depth, type="o", pch=17, col="red3", ylim=c(.67, .71), xlab="Depth", ylab="Test Accuracy", main="SensIT")
        lines(sensit.delayed$classification~sensit.greedy$depth, type="o", pch=19, col="blue3")
        #legend(16, .72, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        #dev.off()
        
        # Protein
        #setEPS()
        #postscript("depth_plots/protein_acc2dep.eps")
        plot(protein.greedy$classification~sensit.greedy$depth, type="o", pch=17, col="red3", ylim=c(.51, .56), xlab="Depth", ylab="Test Accuracy", main="Protein")
        lines(protein.delayed$classification~sensit.greedy$depth, type="o", pch=19, col="blue3")
        #legend(16, .58, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        #dev.off()
        
        # Connect-4
        #setEPS()
        #postscript("depth_plots/c4_acc2dep.eps")
        plot(c4.greedy$classification~sensit.greedy$depth, type="o", pch=17, col="red3", ylim=c(.7, .8), xlab="Depth", ylab="Test Accuracy", main="Connect-4")
        lines(c4.delayed$classification~sensit.greedy$depth, type="o", pch=19, col="blue3")
        #legend(16, .805, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        #dev.off()
        
        plot(mnist.greedy$classification~sensit.greedy$depth, type="o", pch=17, col="red3", ylim=c(.7, .9), xlab="Depth", ylab="Test Accuracy", main="MNIST")
        lines(mnist.delayed$classification~sensit.greedy$depth, type="o", pch=19, col="blue3")
    
        plot.new()
        legend("center", ncol=2, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"), cex=1.5)
        #legend("bottomright", legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"), cex=1.3)
        
        dev.off()
    }
    
    ############ Depth vs. Impurity plots ############
    if(save_imp_plots) {
        setEPS()
        postscript("depth_plots/depth_vs_impurity3x2.eps")
        #layout(matrix(c(1, 2, 3, 4, 5, 6, 7, 7, 7), 3, 3, byrow=TRUE), heights=c(6, 6, 3.8))
        layout(matrix(c(1, 2, 3, 4, 5, 6), 3, 2, byrow=TRUE))#, heights=c(6, 6))
        par(xpd=NA)
        
        # Cars
        #setEPS()
        #postscript("depth_plots/cars_imp2dep.eps")
        plot(cars.greedy$Impurity.deviance~wine.greedy$depth, type="o", pch=17, col="red3", ylim=c(2940, 8220), xlab="Depth", ylab="Deviance", main="Cars")
        lines(cars.delayed$Impurity.deviance~wine.delayed$depth, type="o", pch=19, col="blue3")
        
        print(wine.greedy)
        plot(wine.greedy$Impurity.deviance~wine.greedy$depth, type="o", pch=17, col="red3", ylim=c(375, 600), xlab="Depth", ylab="Impurity", main="Wine")
        lines(wine.delayed$Impurity.deviance~wine.delayed$depth, type="o", pch=19, col="blue3")
        #legend(5, 580, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        #dev.off()
        
        # SensIT
        #setEPS()
        #postscript("depth_plots/sensit_imp2dep.eps")
        plot(sensit.greedy$Impurity.deviance~sensit.greedy$depth, type="o", pch=17, col="red3", ylim=c(14050, 24100), xlab="Depth", ylab="Impurity", main="SensIT")
        lines(sensit.delayed$Impurity.deviance~sensit.greedy$depth, type="o", pch=19, col="blue3")
        #legend(16, 24100, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        #dev.off()
        
        # Protein
        #setEPS()
        #postscript("depth_plots/protein_imp2dep.eps")
        plot(protein.greedy$Impurity.deviance~sensit.greedy$depth, type="o", pch=17, col="red3", ylim=c(4200, 8050), xlab="Depth", ylab="Impurity", main="Protein")
        lines(protein.delayed$Impurity.deviance~sensit.greedy$depth, type="o", pch=19, col="blue3")
        #legend(16, 8050, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        #dev.off()
        
        # Connect-4
        #setEPS()
        #postscript("depth_plots/c4_imp2dep.eps")
        plot(c4.greedy$Impurity.deviance~sensit.greedy$depth, type="o", pch=17, col="red3", ylim=c(8600, 15690), xlab="Depth", ylab="Impurity", main="Connect-4")
        lines(c4.delayed$Impurity.deviance~sensit.greedy$depth, type="o", pch=19, col="blue3")
        #legend(16, 15690, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        #dev.off()
        
        plot(mnist.greedy$Impurity.deviance~sensit.greedy$depth, type="o", pch=17, col="red3", ylim=c(4200, 16000), xlab="Depth", ylab="Impurity", main="MNIST")
        lines(mnist.delayed$Impurity.deviance~sensit.greedy$depth, type="o", pch=19, col="blue3")
        
        #plot.new()
        legend("topright", legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"), cex=1.2)
        
        dev.off()
    }
}
