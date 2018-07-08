run <- function(save_acc_plots=FALSE, save_imp_plots=FALSE) {

    results <- read.csv("./error_to_depth.csv", stringsAsFactors = FALSE)
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
        
        # Wine
        setEPS()
        postscript("depth_plots/wine_acc2dep.eps")
        plot(wine.greedy$classification~wine.greedy$depth, type="o", pch=17, col="red3", ylim=c(.45, .60), xlab="Depth", ylab="Test Accuracy")
        lines(wine.delayed$classification~wine.delayed$depth, type="o", pch=19, col="blue3")
        legend(16, .60, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        dev.off()
        
        # SensIT
        setEPS()
        postscript("depth_plots/sensit_acc2dep.eps")
        plot(sensit.greedy$classification~wine.greedy$depth, type="o", pch=17, col="red3", ylim=c(.65, .72), xlab="Depth", ylab="Test Accuracy")
        lines(sensit.delayed$classification~wine.delayed$depth, type="o", pch=19, col="blue3")
        legend(16, .72, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        dev.off()
        
        # Protein
        setEPS()
        postscript("depth_plots/protein_acc2dep.eps")
        plot(protein.greedy$classification~wine.greedy$depth, type="o", pch=17, col="red3", ylim=c(.5, .58), xlab="Depth", ylab="Test Accuracy")
        lines(protein.delayed$classification~wine.delayed$depth, type="o", pch=19, col="blue3")
        legend(16, .58, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        dev.off()
        
        # Connect-4
        setEPS()
        postscript("depth_plots/c4_acc2dep.eps")
        plot(c4.greedy$classification~wine.greedy$depth, type="o", pch=17, col="red3", ylim=c(.6, .68), xlab="Depth", ylab="Test Accuracy")
        lines(c4.delayed$classification~wine.delayed$depth, type="o", pch=19, col="blue3")
        legend(16, .68, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        dev.off()
        
        # Cars
        setEPS()
        postscript("depth_plots/cars_acc2dep.eps")
        plot(cars.greedy$mae~wine.greedy$depth, type="o", pch=17, col="red3", ylim=c(6.1, 6.35), xlab="Depth", ylab="MAE")
        lines(cars.delayed$mae~wine.delayed$depth, type="o", pch=19, col="blue3")
        legend(16, 6.35, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        dev.off()
    }
    
    ############ Depth vs. Impurity plots ############
    if(save_imp_plots) {
        setEPS()
        postscript("depth_plots/wine_imp2dep.eps")
        plot(wine.greedy$Impurity.deviance~wine.greedy$depth, type="o", pch=17, col="red3", ylim=c(280, 400), xlab="Depth", ylab="Impurity")
        lines(wine.delayed$Impurity.deviance~wine.delayed$depth, type="o", pch=19, col="blue3")
        legend(16, 400, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        dev.off()
        
        # SensIT
        setEPS()
        postscript("depth_plots/sensit_imp2dep.eps")
        plot(sensit.greedy$Impurity.deviance~wine.greedy$depth, type="o", pch=17, col="red3", ylim=c(14050, 24100), xlab="Depth", ylab="Impurity")
        lines(sensit.delayed$Impurity.deviance~wine.delayed$depth, type="o", pch=19, col="blue3")
        legend(16, 24100, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        dev.off()
        
        # Protein
        setEPS()
        postscript("depth_plots/protein_imp2dep.eps")
        plot(protein.greedy$Impurity.deviance~wine.greedy$depth, type="o", pch=17, col="red3", ylim=c(4200, 8050), xlab="Depth", ylab="Impurity")
        lines(protein.delayed$Impurity.deviance~wine.delayed$depth, type="o", pch=19, col="blue3")
        legend(16, 8050, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        dev.off()
        
        # Connect-4
        setEPS()
        postscript("depth_plots/c4_imp2dep.eps")
        plot(c4.greedy$Impurity.deviance~wine.greedy$depth, type="o", pch=17, col="red3", ylim=c(7700, 14500), xlab="Depth", ylab="Impurity")
        lines(c4.delayed$Impurity.deviance~wine.delayed$depth, type="o", pch=19, col="blue3")
        legend(16, 14500, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        dev.off()
        
        # Cars
        setEPS()
        postscript("depth_plots/cars_imp2dep.eps")
        plot(cars.greedy$Impurity.deviance~wine.greedy$depth, type="o", pch=17, col="red3", ylim=c(1750, 1915), xlab="Depth", ylab="Deviance")
        lines(cars.delayed$Impurity.deviance~wine.delayed$depth, type="o", pch=19, col="blue3")
        legend(16, 1915, legend=c("Delayed", "Greedy"), pch=c(19,17), col=c("blue3", "red3"))
        dev.off()
    }
}
